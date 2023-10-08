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

class extractCommand : public console::iCommand {
public:
   virtual void run(console::iLog& l);

   std::string oHash;
   std::string oPath;
};

class myVerb : public console::globalVerb {
protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<extractCommand>("--extract"));

      v->addParameter(
         console::stringParameter::required(offsetof(extractCommand,oHash)));
      v->addParameter(
         console::stringParameter::required(offsetof(extractCommand,oPath)));

      return v.release();
   }
} gVerb;

void extractCommand::run(console::iLog& l)
{
   cms::commonVerb::run(l,[&](){
      tcat::typePtr<cmn::serviceManager> svcMan;
      auto& db = svcMan->demand<db::iDb>();

      db.extract(oHash,cmn::widen(oPath));

      l.writeLnInfo("extract complete");
   });
}

} // anonymous namespace
