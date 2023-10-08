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

class refsCommand : public console::iCommand {
public:
   virtual void run(console::iLog& l);

   std::string oPath;
};

class myVerb : public console::globalVerb {
protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<refsCommand>("--refs"));

      v->addParameter(
         console::stringParameter::required(offsetof(refsCommand,oPath)));

      return v.release();
   }
} gVerb;

void refsCommand::run(console::iLog& l)
{
   cms::commonVerb::run(l,[&](){
      tcat::typePtr<cmn::serviceManager> svcMan;
      auto& db = svcMan->demand<db::iDb>();

      auto c = db.getReferencedClient(cmn::widen(oPath));
      c.lastKnownFolder = oPath;

      tcat::typePtr<db::iHashIndex> hash;
      l.writeLnInfo("indexing assets");
      hash->configure(db);

      db::usageRefs refs;
      refs.guid = c.guid;
      bool valid = true;
      cms::assetFinder::find(cmn::widen(oPath),[&](auto& f)
      {
         l.writeLnInfo("handling file %S",f.fileName().c_str());
         console::autoIndent _ai(l);

         auto h = f.hash();

         if(!hash->hasAsset(h))
         {
            valid = false;
            l.writeLnInfo("FAILED! - asset is unknown");
            console::autoIndent _ai(l);
            l.writeLnInfo("filepath: %S",f.fullFilePath().c_str());
         }
         else
            refs.hashes.insert(h);
      });

      db.saveRefs(refs);
      db.saveClient(c);

      l.writeLnInfo("refs complete");

      db.commit();
   });
}

} // anonymous namespace
