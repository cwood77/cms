#define WIN32_LEAN_AND_MEAN
#include "../cmn/guid.hpp"
#include "../cmn/service.hpp"
#include "../cmn/string.hpp"
#include "../console/arg.hpp"
#include "../console/log.hpp"
#include "../db/api.hpp"
#include "../tcatlib/api.hpp"
#include "commonVerb.hpp"
#include <memory>

namespace {

class registerCommand : public console::iCommand {
public:
   virtual void run(console::iLog& l);

   std::string oPath;
   std::string oName;
};

class myVerb : public console::globalVerb {
protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<registerCommand>("--register"));

      v->addParameter(
         console::stringParameter::required(offsetof(registerCommand,oPath)));
      v->addParameter(
         console::stringParameter::required(offsetof(registerCommand,oName)));

      return v.release();
   }
} gVerb;

void registerCommand::run(console::iLog& l)
{
   cms::commonVerb::run(l,[&](){
      tcat::typePtr<cmn::serviceManager> svcMan;
      auto& db = svcMan->demand<db::iDb>();

      db::client c;
      c.guid = cmn::guid::create();
      l.writeLnInfo("creating new client %s",c.guid.c_str());

      c.name = oName;
      c.lastKnownFolder = oPath;

      db.saveClient(c);
      l.writeLnInfo("register complete");

      db.commit();
   });
}

} // anonymous namespace
