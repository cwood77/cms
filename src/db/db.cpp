#define WIN32_LEAN_AND_MEAN
#include "../cmn/autoPtr.hpp"
#include "../cmn/service.hpp"
#include "../cmn/string.hpp"
#include "../cmn/win32.hpp"
#include "../cms/configHelper.hpp"
#include "../console/log.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include "assetConverter.hpp"
#include "clientConverter.hpp"
#include "usageRefsConverter.hpp"
#include <windows.h>

namespace db {

class db : public iDb {
public:
   db() : m_dirty(false)
   {
      tcat::typePtr<cmn::serviceManager> svcMan;
      m_pLog = &svcMan->demand<console::iLog>();
      m_pConfig = &svcMan->demand<sst::dict>();
   }

   virtual const std::list<asset>& provideAssets() const
   {
      if(m_assets.size())
         return m_assets;

      tcat::typePtr<file::iFileManager> fMan;

      auto aPath = cmn::widen(cms::configHelper::getAppDataPath(*m_pConfig,*m_pLog) + "assets");

      cmn::autoFindHandle hFind;
      WIN32_FIND_DATAW fData;
      hFind.h = ::FindFirstFileW((aPath + L"\\*.sst").c_str(),&fData);
      if(hFind.h == INVALID_HANDLE_VALUE)
         throw std::runtime_error(cmn::narrow(L"bad path: " + aPath));

      do
      {
         if(fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            ;
         else
         {
            std::wstring fullPath = aPath + L"\\" + fData.cFileName;
            cmn::autoReleasePtr<file::iSstFile> pFile(&fMan->bindFile<file::iSstFile>(
               cmn::narrow(fullPath).c_str(),
               file::iFileManager::kReadOnly
            ));
            pFile->tie(*m_pLog);
            tcat::typePtr<iAssetConverter> fmt;
            asset a;
            fmt->loadFromSst(pFile->dict(),a);
            m_assets.push_back(a);
         }
      } while(::FindNextFileW(hFind.h,&fData));

      return m_assets;
   }

   virtual void saveClient(const client& c)
   {
      auto appPath = cms::configHelper::getAppDataPath(*m_pConfig,*m_pLog);
      auto sstPath = appPath + "clients\\" + c.guid + ".sst";

      tcat::typePtr<file::iFileManager> fMan;

      // save the SST
      cmn::autoReleasePtr<file::iSstFile> pFile(&fMan->bindFile<file::iSstFile>(
         sstPath.c_str()
      ));
      pFile->tie(*m_pLog);
      tcat::typePtr<iClientConverter> fmt;
      fmt->saveToSst(c,pFile->dict());

      // commit the SST
      pFile->scheduleFor(file::iFileManager::kSaveOnClose);
      m_dirty = true;

      saveClientRef(c);
   }

   virtual std::list<client> listClients() const
   {
      tcat::typePtr<file::iFileManager> fMan;

      auto cPath = cmn::widen(cms::configHelper::getAppDataPath(*m_pConfig,*m_pLog) + "clients");

      cmn::autoFindHandle hFind;
      WIN32_FIND_DATAW fData;
      hFind.h = ::FindFirstFileW((cPath + L"\\*.sst").c_str(),&fData);
      if(hFind.h == INVALID_HANDLE_VALUE)
         throw std::runtime_error(cmn::narrow(L"bad path: " + cPath));

      std::list<client> rval;
      do
      {
         if(fData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            ;
         else
         {
            std::wstring fullPath = cPath + L"\\" + fData.cFileName;
            cmn::autoReleasePtr<file::iSstFile> pFile(&fMan->bindFile<file::iSstFile>(
               cmn::narrow(fullPath).c_str(),
               file::iFileManager::kReadOnly
            ));
            pFile->tie(*m_pLog);
            tcat::typePtr<iClientConverter> fmt;
            client c;
            fmt->loadFromSst(pFile->dict(),c);
            rval.push_back(c);
         }
      } while(::FindNextFileW(hFind.h,&fData));

      return rval;
   }

   virtual client getReferencedClient(const std::wstring& refPath) const
   {
      clientRef r;
      {
         std::wstring fullPath = refPath + L"\\cms.sst";
         tcat::typePtr<file::iFileManager> fMan;
         cmn::autoReleasePtr<file::iSstFile> pFile(&fMan->bindFile<file::iSstFile>(
            cmn::narrow(fullPath).c_str(),
            file::iFileManager::kReadOnly
         ));
         pFile->tie(*m_pLog);
         tcat::typePtr<iClientRefConverter> fmt;
         fmt->loadFromSst(pFile->dict(),r);
      }

      auto clients = listClients();
      for(auto client : clients)
         if(client.guid == r.guid)
            return client;

      throw std::runtime_error("no client for refering guid!");
   }

   virtual usageRefs getUsageRefs(const client& c) const
   {
      tcat::typePtr<file::iFileManager> fMan;

      auto appPath = cms::configHelper::getAppDataPath(*m_pConfig,*m_pLog);
      auto sstPath = appPath + "usages\\" + c.guid + ".sst";

      if(!fMan->doesFileExist(sstPath))
         return usageRefs(); // not an error

      cmn::autoReleasePtr<file::iSstFile> pFile(&fMan->bindFile<file::iSstFile>(
         sstPath.c_str(),
         file::iFileManager::kReadOnly
      ));
      pFile->tie(*m_pLog);
      usageRefs r;
      tcat::typePtr<iUsageRefsConverter> fmt;
      fmt->loadFromSst(pFile->dict(),r);

      return r;
   }

   virtual void publish(const asset& a, const std::wstring& fullAssetPath, const std::wstring& fullThumbnailPath)
   {
      m_assets.clear();

      auto appPath = cms::configHelper::getAppDataPath(*m_pConfig,*m_pLog);
      auto sstPath = appPath + "assets\\" + a.hash + ".sst";
      auto assetPath
         = cmn::widen(appPath + "assets\\" + a.hash + ".")
         + cmn::splitExt(fullAssetPath);
      std::wstring thumbnailPath;
      if(!fullThumbnailPath.empty())
         thumbnailPath
            = cmn::widen(appPath + "assets\\" + a.hash + "-tn.")
            + cmn::splitExt(fullThumbnailPath);

      tcat::typePtr<file::iFileManager> fMan;
      if(fMan->doesFileExist(sstPath))
      {
         m_pLog->writeLnInfo("asset already recorded");
         return;
      }

      // save the SST
      cmn::autoReleasePtr<file::iSstFile> pFile(&fMan->bindFile<file::iSstFile>(
         sstPath.c_str()
      ));
      pFile->tie(*m_pLog);
      tcat::typePtr<iAssetConverter> fmt;
      fmt->saveToSst(a,pFile->dict());

      // copy asset
      auto success = ::CopyFileW(
         fullAssetPath.c_str(),
         assetPath.c_str(),
         /*bFailIfExists*/TRUE);
      if(!success)
         throw std::runtime_error("failed to copy asset file");

      // copy thumbnail
      if(!fullThumbnailPath.empty())
      {
         auto success = ::CopyFileW(
            fullThumbnailPath.c_str(),
            thumbnailPath.c_str(),
            /*bFailIfExists*/TRUE);
         if(!success)
            throw std::runtime_error("failed to copy thumbnail file");
      }

      // commit the SST
      pFile->scheduleFor(file::iFileManager::kSaveOnClose);
      m_dirty = true;
   }

   virtual void erase(const asset& a)
   {
      m_assets.clear();

      m_pLog->writeLnInfo("deleting asset %s",a.hash.c_str());

      auto appPath = cms::configHelper::getAppDataPath(*m_pConfig,*m_pLog);
      auto sstPath = appPath + "assets\\" + a.hash + ".sst";
      auto assetPath
         = appPath + "assets\\" + a.hash + "."
         + cmn::narrow(cmn::splitExt(cmn::widen(a.fileName)));
      std::string thumbnailPath;
      if(!a.thumbnailExt.empty())
         thumbnailPath
            = appPath + "assets\\" + a.hash + "-tn."
            + a.thumbnailExt;

      auto success = ::DeleteFileA(sstPath.c_str());
      if(!success)
         m_pLog->writeLnInfo("warning: FAILED to delete sst file");

      if(!thumbnailPath.empty())
      {
         success = ::DeleteFileA(thumbnailPath.c_str());
         if(!success)
            m_pLog->writeLnInfo("warning: FAILED to delete thumbnail");
      }

      success = ::DeleteFileA(assetPath.c_str());
      if(!success)
         m_pLog->writeLnInfo("warning: FAILED to delete asset");

      m_dirty = true;
   }

   virtual asset loadAsset(const std::string& hash) const
   {
      tcat::typePtr<file::iFileManager> fMan;

      auto aPath = cms::configHelper::getAppDataPath(*m_pConfig,*m_pLog) + "assets";
      auto fullPath = aPath + "\\" + hash + ".sst";

      cmn::autoReleasePtr<file::iSstFile> pFile(&fMan->bindFile<file::iSstFile>(
         fullPath.c_str(),
         file::iFileManager::kReadOnly
      ));
      pFile->tie(*m_pLog);
      tcat::typePtr<iAssetConverter> fmt;
      asset a;
      fmt->loadFromSst(pFile->dict(),a);

      return a;
   }

   virtual void extract(const std::string& hash, const std::wstring& destPath) const
   {
      auto appPath = cms::configHelper::getAppDataPath(*m_pConfig,*m_pLog);
      auto sstPath = appPath + "assets\\" + hash + ".sst";

      tcat::typePtr<file::iFileManager> fMan;
      if(!fMan->doesFileExist(sstPath))
      {
         m_pLog->writeLnInfo("no asset of this hash exists");
         return;
      }

      cmn::autoReleasePtr<file::iSstFile> pFile(&fMan->bindFile<file::iSstFile>(
         sstPath.c_str(),
         file::iFileManager::kReadOnly
      ));
      pFile->tie(*m_pLog);
      tcat::typePtr<iAssetConverter> fmt;
      asset a;
      fmt->loadFromSst(pFile->dict(),a);

      auto fullDestPath
         = destPath + L"\\"
         + cmn::widen(a.fileName);
      auto fullSrcPath
         = cmn::widen(appPath + "assets\\" + hash + ".")
         + cmn::splitExt(fullDestPath);

      m_pLog->writeLnInfo("copying '%S' to '%S'",fullSrcPath.c_str(),fullDestPath.c_str());

      auto success = ::CopyFileW(
         fullSrcPath.c_str(),
         fullDestPath.c_str(),
         /*bFailIfExists*/TRUE);
      if(!success)
         throw std::runtime_error("failed to copy asset file");
   }

   virtual void saveRefs(const usageRefs& r)
   {
      tcat::typePtr<file::iFileManager> fMan;

      auto appPath = cms::configHelper::getAppDataPath(*m_pConfig,*m_pLog);
      auto sstPath = appPath + "usages\\" + r.guid + ".sst";

      // save the SST
      cmn::autoReleasePtr<file::iSstFile> pFile(&fMan->bindFile<file::iSstFile>(
         sstPath.c_str()
      ));
      pFile->tie(*m_pLog);
      tcat::typePtr<iUsageRefsConverter> fmt;
      fmt->saveToSst(r,pFile->dict());

      // commit the SST
      pFile->scheduleFor(file::iFileManager::kSaveOnClose);
   }

   virtual void commit()
   {
      if(!m_dirty)
         ;//return;

      tcat::typeSet<iDbObserver> pObs;
      for(size_t i=0;i<pObs.size();i++)
         pObs[i]->onSave(*this);
   }

private:
   void saveClientRef(const client& c)
   {
      auto sstPath = c.lastKnownFolder + "\\cms.sst";

      tcat::typePtr<file::iFileManager> fMan;
      if(fMan->doesFileExist(sstPath))
         return;

      clientRef r;
      r.guid = c.guid;

      // save the SST
      cmn::autoReleasePtr<file::iSstFile> pFile(&fMan->bindFile<file::iSstFile>(
         sstPath.c_str()
      ));
      pFile->tie(*m_pLog);
      tcat::typePtr<iClientRefConverter> fmt;
      fmt->saveToSst(r,pFile->dict());

      // commit the SST
      pFile->scheduleFor(file::iFileManager::kSaveOnClose);
   }

   console::iLog *m_pLog;
   sst::dict *m_pConfig;
   mutable std::list<asset> m_assets;
   bool m_dirty;
};

tcatExposeSingletonTypeAs(db,iDb);

} // namespace db

tcatImplServer();

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
