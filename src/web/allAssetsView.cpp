#include "../cmn/service.hpp"
#include "../cmn/string.hpp"
#include "../cms/configHelper.hpp"
#include "../console/log.hpp"
#include "../db/api.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../tcatlib/api.hpp"
#include "html.hpp"
#include "htmlHelper.hpp"
#include "iView.hpp"
#include <fstream>

namespace web {
namespace {

class allAssetsViewWriter : public db::iDbObserver, public iView {
public:
   allAssetsViewWriter()
   {
      tcat::typePtr<cmn::serviceManager> svcMan;
      m_pLog = &svcMan->demand<console::iLog>();
      m_pConfig = &svcMan->demand<sst::dict>();
   }

   virtual void onSave(db::iDb& db)
   {
      html::root r;
      auto& b = htmlWriterHelper::boilerplate(r,*this);

      auto& assets = db.provideAssets();
      std::map<std::string,std::list<db::asset> > sorted;
      for(auto& asset : assets)
         for(auto& tag : asset.tags)
            sorted[tag].push_back(asset);
      for(auto it=sorted.begin();it!=sorted.end();++it)
      {
         b.addChild<html::h2>()
            .addChild<html::content>()
               .content << it->first;

         htmlWriterHelper::assetTable(it->second,b);
      }

      tcat::typePtr<file::iFileManager> fMan;
      auto wPath = cms::configHelper::getAppDataPath(*m_pConfig,*m_pLog) + "www\\index.html";
      fMan->createAllFoldersForFile(wPath.c_str(),*m_pLog,/*really?*/true);
      std::ofstream o(wPath.c_str());
      html::htmlStream s(o);
      r.write(s);

      m_pLog->writeLnInfo("to view db visit '%s'",wPath.c_str());
   }

   virtual std::string name() const { return "all assets"; }
   virtual std::string linkTarget() const { return "index.html"; }

private:
   console::iLog *m_pLog;
   sst::dict *m_pConfig;
};

tcatExposeTypeAs(allAssetsViewWriter,db::iDbObserver);
namespace {
tcatExposeTypeAs(allAssetsViewWriter,iView);
} // anonymous namespace
} // anonymous namespace
} // namespace web
