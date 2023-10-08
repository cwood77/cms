#define WIN32_LEAN_AND_MEAN
#include "../cmn/guid.hpp"
#include "../cmn/service.hpp"
#include "../cmn/string.hpp"
#include "../console/arg.hpp"
#include "../console/log.hpp"
#include "../db/api.hpp"
#include "../tcatlib/api.hpp"
#include "commonVerb.hpp"
#include "finder.hpp"
#include <memory>

namespace {

class deleteCommand : public console::iCommand {
public:
   virtual void run(console::iLog& l);

   std::string oTag;
};

class myVerb : public console::globalVerb {
protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<deleteCommand>("--delete"));

      v->addParameter(
         console::stringParameter::required(offsetof(deleteCommand,oTag)));

      return v.release();
   }
} gVerb;

void deleteCommand::run(console::iLog& l)
{
   cms::commonVerb::run(l,[&](){
      tcat::typePtr<cmn::serviceManager> svcMan;
      auto& db = svcMan->demand<db::iDb>();

      tcat::typePtr<db::iTagFilter> filter;
      l.writeLnInfo("filtering assets");
      filter->configure(oTag,db);

      auto& assets = filter->provideAssets();
      for(auto asset : assets)
         db.erase(asset);

      l.writeLnInfo("deleted %lld asset(s)",assets.size());

      db.commit();
   });
}

} // anonymous namespace
