#define WIN32_LEAN_AND_MEAN
#include "../cmn/autoPtr.hpp"
#include "../cmn/guid.hpp"
#include "../cmn/service.hpp"
#include "../cmn/string.hpp"
#include "../console/arg.hpp"
#include "../console/log.hpp"
#include "../db/api.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../tcatlib/api.hpp"
#include "finder.hpp"
#include <memory>

namespace {

class publishCommand : public console::iCommand {
public:
   publishCommand() : oPreview(false) {}

   virtual void run(console::iLog& l);

   std::string oPath;
   std::string oSource;
   std::string oTags;

   bool oPreview;
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
         *new console::boolOption("--preview",offsetof(publishCommand,oPreview)));

      return v.release();
   }
} gVerb;

void publishCommand::run(console::iLog& l)
{
   tcat::typePtr<file::iFileManager> fMan;
   cmn::autoReleasePtr<file::iSstFile> pFile(&fMan->bindFile<file::iSstFile>(
      file::iFileManager::kExeAdjacent,
      "config.sst",
      file::iFileManager::kReadOnly
   ));
   pFile->tie(l);
   l.configure(pFile->dict());

   l.writeLnDebug("compiling services");
   tcat::typePtr<cmn::serviceManager> svcMan;
   cmn::autoService<console::iLog> _alog(*svcMan,l);
   cmn::autoService<sst::dict> _afig(*svcMan,pFile->dict());

   l.writeLnDebug("loading DB");
   tcat::typePtr<db::iDb> db;
   cmn::autoService<db::iDb> _adb(*svcMan,*db);

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

      a.hash = f.hash();
      a.fileName = cmn::narrow(f.fileName());

      db->publish(a,f.fullFilePath());
   });

   l.writeLnInfo("publish complete");
}

} // anonymous namespace
