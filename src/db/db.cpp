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
#include "usageRefsConverter.hpp"
#include <windows.h>

namespace db {

class db : public iDb {
public:
   db()
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

   virtual void publish(const asset& a, const std::wstring& fullAssetPath)
   {
      m_assets.clear();

      auto appPath = cms::configHelper::getAppDataPath(*m_pConfig,*m_pLog);
      auto sstPath = appPath + "assets\\" + a.hash + ".sst";
      auto assetPath
         = cmn::widen(appPath + "assets\\" + a.hash + ".")
         + cmn::splitExt(fullAssetPath);

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

      // commit the SST
      pFile->scheduleFor(file::iFileManager::kSaveOnClose);
   }

   virtual void saveRefs(const usageRefs& r, const std::string& path)
   {
      tcat::typePtr<file::iFileManager> fMan;

      // save the SST
      cmn::autoReleasePtr<file::iSstFile> pFile(&fMan->bindFile<file::iSstFile>(
         (path + "\\usageRefs.sst").c_str()
      ));
      pFile->tie(*m_pLog);
      tcat::typePtr<iUsageRefsConverter> fmt;
      fmt->saveToSst(r,pFile->dict());

      // commit the SST
      pFile->scheduleFor(file::iFileManager::kSaveOnClose);
   }

private:
   console::iLog *m_pLog;
   sst::dict *m_pConfig;
   mutable std::list<asset> m_assets;
};

tcatExposeSingletonTypeAs(db,iDb);

} // namespace db

tcatImplServer();

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
