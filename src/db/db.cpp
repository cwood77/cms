#define WIN32_LEAN_AND_MEAN
#include "../cmn/autoPtr.hpp"
#include "../cmn/service.hpp"
#include "../cmn/string.hpp"
#include "../cmn/win32.hpp"
#include "../cms/configHelper.hpp"
#include "../console/log.hpp"
#include "../db/assetConverter.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include <cstdio>
#include <stdarg.h>
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
      throw 3.14;
   }

   virtual void publish(const asset& a, const std::wstring& fullAssetPath)
   {
      auto appPath = cms::configHelper::getAppDataPath(*m_pConfig,*m_pLog);
      auto sstPath = appPath + "assets\\" + a.hash + ".sst";
      auto assetPath
         = cmn::widen(appPath + "assets\\" + a.hash + ".")
         + cmn::splitExt(fullAssetPath);

      tcat::typePtr<file::iFileManager> fMan;
      if(fMan->doesFileExist(sstPath))
      {
         m_pLog->writeLnVerbose("asset already recorded");
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

private:
   console::iLog *m_pLog;
   sst::dict *m_pConfig;
};

tcatExposeSingletonTypeAs(db,iDb);

} // namespace db

tcatImplServer();

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) { return TRUE; }
