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
   fileInfo(const std::wstring& fullFilePath, const std::wstring& fileName)
   : m_fileName(fileName), m_fullFilePath(fullFilePath) {}

   virtual const std::wstring& fileName() const { return m_fileName; }
   virtual const std::wstring& fullFilePath() const { return m_fullFilePath; }

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
         l.writeLnVerbose("computed hash is %s",m_hash.c_str());
      }
      return m_hash;
   }

private:
   std::wstring m_fileName;
   std::wstring m_fullFilePath;
   mutable std::string m_hash;
};

} // anonymous namespace

assetInfo::assetInfo()
{
   m_exts.insert(L"avif");
   m_exts.insert(L"jfif");
   m_exts.insert(L"jpg");
   m_exts.insert(L"png");
   m_exts.insert(L"webp");
}

zipHandler::zipHandler()
{
   tcat::typePtr<cmn::serviceManager> svcMan;
   m_pLog = &svcMan->demand<console::iLog>();
}

void zipHandler::noteFile(const std::wstring& fullPath, const std::wstring& fileName)
{
   auto ext = cmn::lower(cmn::splitExt(fileName));
   if(ext != L"zip")
      return;

   std::wstring without;
   cmn::splitExt(fullPath,&without);
   m_foldersToIgnore.insert(without);
}

bool zipHandler::shouldDescendIntoFolder(const std::wstring& fullPath) const
{
   const bool skip = (m_foldersToIgnore.find(fullPath)!=m_foldersToIgnore.end());

   if(skip)
      m_pLog->writeLnVerbose("skipping expanded ZIP file - %S",fullPath.c_str());

   return !skip;
}

void assetFinder::find(const std::wstring& path, std::function<void(iFileInfo&)> f)
{
   assetFinder(f).find(path);
}

assetFinder::assetFinder(std::function<void(iFileInfo&)> f)
: m_f(f)
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
   m_pLog->writeLnVerbose("traversing folder %S", path.c_str());
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
   m_zip.noteFile(fullPath,fileName);
   auto ext = cmn::lower(cmn::splitExt(fileName));
   if(!m_aInfo.isExt(ext))
   {
      m_pLog->writeLnVerbose("Ignoring file ext %S (%S)",ext.c_str(),fullPath.c_str());
      return;
   }

   fileInfo fi(fullPath,fileName);
   m_f(fi);
}

} // namespace cms
