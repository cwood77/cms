#include "../cmn/hash.hpp"
#include "../cmn/service.hpp"
#include "../cmn/string.hpp"
#include "../cmn/win32.hpp"
#include "../console/log.hpp"
#include "../tcatlib/api.hpp"
#include "finder.hpp"
#include <stdexcept>

namespace cms {
namespace {

class fileInfo : public assetFinder::iFileInfo {
public:
   fileInfo(const std::wstring& fullFilePath, const std::wstring& fileName, const std::wstring& ext, const std::wstring& thumbnail = L"")
   : m_fileName(fileName), m_ext(ext), m_fullFilePath(fullFilePath), m_thumbnailPath(thumbnail) {}

   virtual const std::wstring& fileName() const { return m_fileName; }
   virtual const std::wstring& ext() const { return m_ext; }
   virtual const std::wstring& fullFilePath() const { return m_fullFilePath; }
   virtual const std::wstring& thumbnailFullFilePath() const { return m_thumbnailPath; }

   virtual const std::string& hash() const
   {
      if(m_hash.empty())
      {
         cmn::autoCryptoContext ctxt;
         cmn::md5Hasher hasher(ctxt);
         hasher.addFile(cmn::narrow(m_fullFilePath));
         cmn::md5Hash hash;
         hasher.get(hash);
         m_hash = hash.toString();

         tcat::typePtr<cmn::serviceManager> svcMan;
         auto& l = svcMan->demand<console::iLog>();
         l.writeLnInfo("computed hash is %s",m_hash.c_str());
      }
      return m_hash;
   }

private:
   std::wstring m_fileName;
   std::wstring m_ext;
   std::wstring m_fullFilePath;
   std::wstring m_thumbnailPath;
   mutable std::string m_hash;
};

} // anonymous namespace

zipHandler::zipHandler()
{
   tcat::typePtr<cmn::serviceManager> svcMan;
   m_pLog = &svcMan->demand<console::iLog>();
}

bool zipHandler::noteFile(const std::wstring& fullPath, const std::wstring& fileName)
{
   auto ext = cmn::lower(cmn::splitExt(fileName));
   if(ext != L"zip")
      return false;

   std::wstring without;
   cmn::splitExt(fullPath,&without);
   m_foldersToHandle[without];
   m_fileNames[without] = fileName;
   return true;
}

bool zipHandler::shouldDescendIntoFolder(const std::wstring& fullPath)
{
   const bool handle = (m_foldersToHandle.find(fullPath)!=m_foldersToHandle.end());

   if(handle)
   {
      m_pLog->writeLnInfo("looking for thumbnail in expanded ZIP file - %S",fullPath.c_str());

      console::autoIndent _i(*m_pLog);
      tcat::typePtr<db::iAssetFileTypeExpert> aFTEx;
      std::wstring tn;
      assetFinder::find(fullPath,[&](auto& f)
      {
         if(tn.empty() && aFTEx->fetch(f.ext())->isWebViewable())
            tn = f.fullFilePath();
      },/*chatty*/false);
      if(!tn.empty())
      {
         m_pLog->writeLnInfo("found thumbnail %S",tn.c_str());
         m_foldersToHandle[fullPath] = tn;
      }
   }

   return !handle;
}

void zipHandler::processDeferredAdds(std::function<void(const std::wstring&, const std::wstring&, const std::wstring&)> f)
{
   for(auto it=m_foldersToHandle.begin();it!=m_foldersToHandle.end();++it)
      f(it->first,m_fileNames[it->first],it->second);
}

void assetFinder::find(const std::wstring& path, std::function<void(iFileInfo&)> f, bool chatty)
{
   assetFinder self(f,chatty);
   self.find(path);
   self.m_zip.processDeferredAdds([&](auto& fullFolderPath, auto& fileName, auto& tn)
   {
      fileInfo fi(fullFolderPath + L".zip",fileName,L"zip",tn);
      f(fi);
   });
}

assetFinder::assetFinder(std::function<void(iFileInfo&)> f, bool chatty)
: m_chatty(chatty)
, m_f(f)
{
   tcat::typePtr<cmn::serviceManager> svcMan;
   m_pLog = &svcMan->demand<console::iLog>();
}

void assetFinder::find(const std::wstring& path)
{
   cmn::autoFindHandle hFind;
   WIN32_FIND_DATAW fData;
   hFind.h = ::FindFirstFileW((path + L"\\*").c_str(),&fData);
   if(hFind.h == INVALID_HANDLE_VALUE)
      throw std::runtime_error(cmn::narrow(L"bad path: " + path));

   std::set<std::wstring> subfolders;
   m_pLog->writeLnInfo("traversing folder %S", path.c_str());
   do
   {
      if(std::wstring(L".") == fData.cFileName)
         continue;
      if(std::wstring(L"..") == fData.cFileName)
         continue;

      std::wstring fullPath = path + L"\\" + fData.cFileName;

      if(fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
         subfolders.insert(fullPath);
      else
         considerFile(fullPath,fData.cFileName);
   } while(::FindNextFileW(hFind.h,&fData));

   for(auto& s : subfolders)
      if(m_zip.shouldDescendIntoFolder(s))
         find(s);
}

void assetFinder::considerFile(const std::wstring& fullPath, const std::wstring& fileName)
{
   if(m_zip.noteFile(fullPath,fileName))
      return; // deferring add for later, in case I can find a thumbnail

   auto ext = cmn::lower(cmn::splitExt(fileName));
   if(m_pAExpert->fetch(ext) == NULL)
   {
      if(m_chatty)
         m_pLog->writeLnInfo("Ignoring file ext %S (%S)",ext.c_str(),fullPath.c_str());
      return;
   }

   fileInfo fi(fullPath,fileName,ext);
   m_f(fi);
}

} // namespace cms
