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

class clientsViewWriter : public db::iDbObserver, public iView {
public:
   clientsViewWriter()
   {
      tcat::typePtr<cmn::serviceManager> svcMan;
      m_pLog = &svcMan->demand<console::iLog>();
      m_pConfig = &svcMan->demand<sst::dict>();
   }

   virtual void onSave(db::iDb& db)
   {
      html::root r;
      auto& b = htmlWriterHelper::boilerplate(r,*this);

      auto clients = db.listClients();

      b.addChild<html::content>()
         .content << clients.size() << " client(s)";

      for(auto& client : clients)
      {
         b.addChild<html::h2>()
            .addChild<html::content>()
               .content << client.name;

         b.addChild<html::content>()
            .content << "guid: " << client.guid;
         b.addChild<html::br>();
         b.addChild<html::br>();

         auto refs = db.getUsageRefs(client);
         b.addChild<html::content>()
            .content << refs.hashes.size() << " referenced asset(s):";

         std::list<db::asset> assets;
         for(auto h : refs.hashes)
            assets.push_back(db.loadAsset(h));

         if(assets.size()==0)
            b.addChild<html::br>()
               .addChild<html::content>()
                  .content << "No references!";

         auto oLegal = htmlWriterHelper::assetTable(assets,b);

         b.addChild<html::content>()
            .content << "overall: " << oLegal;
      }

      auto wPath = cms::configHelper::getAppDataPath(*m_pConfig,*m_pLog) + "www\\clients.html";
      tcat::typePtr<file::iFileManager> fMan;
      fMan->createAllFoldersForFile(wPath.c_str(),*m_pLog,/*really?*/true);
      std::ofstream o(wPath.c_str());
      html::htmlStream s(o);
      r.write(s);

      m_pLog->writeLnInfo("to view db visit '%s'",wPath.c_str());
   }

   virtual std::string name() const { return "clients"; }
   virtual std::string linkTarget() const { return "clients.html"; }

private:
   console::iLog *m_pLog;
   sst::dict *m_pConfig;
};

tcatExposeTypeAs(clientsViewWriter,db::iDbObserver);
namespace {
tcatExposeTypeAs(clientsViewWriter,iView);
} // anonymous namespace
} // anonymous namespace
} // namespace web
