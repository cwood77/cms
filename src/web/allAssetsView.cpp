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
      tcat::typePtr<file::iFileManager> fMan;
      auto aPath = cms::configHelper::getAppDataPath(*m_pConfig,*m_pLog) + "assets";
      auto wPath = cms::configHelper::getAppDataPath(*m_pConfig,*m_pLog) + "www\\index.html";

      html::root r;
      auto& b = htmlWriterHelper::boilerplate(r,*this);

      tcat::typePtr<db::iAssetFileTypeExpert> aFTEx;

      auto& assets = db.provideAssets();
      std::map<std::string,std::map<std::string,const db::asset*> > sorted;
      for(auto& asset : assets)
         sorted[*asset.tags.begin()][asset.fileName] = &asset;
      for(auto it=sorted.begin();it!=sorted.end();++it)
      {
         b.addChild<html::h2>()
            .addChild<html::content>()
               .content << it->first;

         auto& t = b.addChild<html::table>();
         t.attrs["border"]="1";
         {
            auto& r = t.addChild<html::tr>();
            r.addChild<html::td>()
               .addChild<html::content>().content << "thumbnail";
            r.addChild<html::td>()
               .addChild<html::content>().content << "filename";
            r.addChild<html::td>()
               .addChild<html::content>().content << "source";
            r.addChild<html::td>()
               .addChild<html::content>().content << "hash";
         }

         auto& f = it->second;
         for(auto jit=f.begin();jit!=f.end();++jit)
         {
            auto& r = t.addChild<html::tr>();
            auto& tn = r.addChild<html::td>();
            {
               auto ext = cmn::lower(cmn::splitExt(cmn::widen(jit->first)));
               auto *pFT = aFTEx->fetch(ext);
               if(pFT->isWebViewable())
               {
                  auto& i = tn.addChild<html::img>();
                  i.attrs["src"] = "../assets/" + jit->second->hash + "." + cmn::narrow(ext);
                  i.attrs["style"] = "max-height: 100px; max-width: 100px;";
               }
               else if(!jit->second->thumbnailExt.empty())
               {
                  // try a thumbnail instead
                  auto& i = tn.addChild<html::img>();
                  i.attrs["src"] = "../assets/" + jit->second->hash + "-tn." + jit->second->thumbnailExt;
                  i.attrs["style"] = "max-height: 100px; max-width: 100px;";
               }
            }
            r.addChild<html::td>()
               .addChild<html::content>().content << jit->first;
            r.addChild<html::td>()
               .addChild<html::content>().content << jit->second->source;
            r.addChild<html::td>()
               .addChild<html::content>().content << jit->second->hash;
         }
      }

      fMan->createAllFoldersForFile(wPath.c_str(),*m_pLog,/*really?*/true);
      std::ofstream o(wPath.c_str());
      html::htmlStream s(o);
      r.write(s);

      m_pLog->writeLnInfo("to view db visit '%s'",wPath.c_str());
   }

   virtual std::string name() const { return "all assets"; }
   virtual std::string linkTarget() const { return "TBD"; }

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
