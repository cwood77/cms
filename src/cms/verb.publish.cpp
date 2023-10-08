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

class publishCommand : public console::iCommand {
public:
   publishCommand() : oLegal(false), oIllegal(false) {}

   virtual void run(console::iLog& l);

   std::string oPath;
   std::string oSource;
   std::string oTags;
   bool oLegal;
   bool oIllegal;
};

class myVerb : public console::globalVerb {
protected:
   virtual console::verbBase *inflate()
   {
      std::unique_ptr<console::verbBase> v(
         new console::verb<publishCommand>("--publish"));

      v->addParameter(
         console::stringParameter::required(offsetof(publishCommand,oPath)));
      v->addParameter(
         console::stringParameter::required(offsetof(publishCommand,oSource)));
      v->addParameter(
         console::stringParameter::required(offsetof(publishCommand,oTags)));

      v->addOption(
         *new console::boolOption("--legal",offsetof(publishCommand,oLegal)));
      v->addOption(
         *new console::boolOption("--illegal",offsetof(publishCommand,oIllegal)));

      return v.release();
   }
} gVerb;

void publishCommand::run(console::iLog& l)
{
   cms::commonVerb::run(l,[&](){
      tcat::typePtr<cmn::serviceManager> svcMan;
      auto& db = svcMan->demand<db::iDb>();

      auto tags = cmn::splitSet(oTags,',');

      cms::assetFinder::find(cmn::widen(oPath),[&](auto& f)
      {
         l.writeLnInfo("handling file %S",f.fileName().c_str());
         console::autoIndent _ai(l);

         db::asset a;
         a.guid = cmn::guid::create();
         l.writeLnInfo("creating new asset %s",a.guid.c_str());

         a.source = oSource;
         a.tags = tags;
         if(oLegal)
            a.legal = "legal";
         else if(oIllegal)
            a.legal = "illegal";
         else
            a.legal = "unknown";

         a.hash = f.hash();
         a.fileName = cmn::narrow(f.fileName());
         if(!f.thumbnailFullFilePath().empty())
            a.thumbnailExt = cmn::narrow(cmn::splitExt(f.thumbnailFullFilePath()));

         db.publish(a,f.fullFilePath(),f.thumbnailFullFilePath());
      });

      l.writeLnInfo("publish complete");

      db.commit();
   });
}

} // anonymous namespace
