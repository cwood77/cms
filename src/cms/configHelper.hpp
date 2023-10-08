#ifndef ___cms_configHelper___
#define ___cms_configHelper___

#include "../file/api.hpp"
#include "../file/manager.hpp"
#include "../tcatlib/api.hpp"

namespace console { class iLog; }

namespace cms {

class configHelper {
public:
   static std::string getAppDataPath(sst::dict& fig, console::iLog& l)
   {
      tcat::typePtr<file::iFileManager> fMan;

      auto dbPath = fig.getOpt<sst::str>("appdata-path","");
      if(!dbPath.empty())
         return initAppDataIf(*fMan,l,dbPath);

      auto *pPath = fMan->calculatePath(file::iFileManager::kUserData,"");
      return initAppDataIf(*fMan,l,pPath);
   }

private:
   static std::string initAppDataIf(file::iFileManager& fMan, console::iLog& l, const std::string& root)
   {
      fMan.createAllFoldersForFolder((root + "assets").c_str(),l,/*really*/true);
      fMan.createAllFoldersForFolder((root + "www").c_str(),l,/*really*/true);
      fMan.createAllFoldersForFolder((root + "clients").c_str(),l,/*really*/true);
      fMan.createAllFoldersForFolder((root + "usages").c_str(),l,/*really*/true);
      return root;
   }
};

} // namespace cms

#endif // ___cms_configHelper___
