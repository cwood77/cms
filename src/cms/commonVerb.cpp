#include "../cmn/autoPtr.hpp"
#include "../cmn/service.hpp"
#include "../console/log.hpp"
#include "../db/api.hpp"
#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../tcatlib/api.hpp"
#include "commonVerb.hpp"

namespace cms {

void commonVerb::run(console::iLog& l, std::function<void()> f)
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

   f();
}

} // namespace cms
