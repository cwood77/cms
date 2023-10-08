#include "../cmn/string.hpp"
#include "../file/api.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include "assetConverter.hpp"
#include <sstream>

namespace db {

class assetConverter : public iAssetConverter {
public:
   virtual void saveToSst(const asset& a, sst::dict& d)
   {
      d.add<sst::str>("guid") = a.guid;
      d.add<sst::str>("source") = a.source;
      {
         bool first = true;
         std::stringstream tags;
         for(auto& t : a.tags)
         {
            if(!first)
               tags << ",";
            tags << t;
            first = false;
         }
         d.add<sst::str>("tags") = tags.str();
      }
      d.add<sst::str>("legal") = a.legal;
      d.add<sst::str>("hash") = a.hash;
      d.add<sst::str>("fileName") = a.fileName;
      d.add<sst::str>("thumbnailExt") = a.thumbnailExt;
   }

   virtual void loadFromSst(sst::dict& d, asset& a)
   {
      a.guid = d["guid"].as<sst::str>().get();
      a.source = d["source"].as<sst::str>().get();
      std::string T = d["tags"].as<sst::str>().get();
      a.tags = cmn::splitSet(T,',');
      a.legal = d.getOpt<sst::str>("legal","unknown");
      a.hash = d["hash"].as<sst::str>().get();
      a.fileName = d["fileName"].as<sst::str>().get();
      a.thumbnailExt = d.getOpt<sst::str>("thumbnailExt","");
   }
};

tcatExposeSingletonTypeAs(assetConverter,iAssetConverter);

} // namespace db
