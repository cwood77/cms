#include "../db/api.hpp"
#include "../file/api.hpp"
#include "../tcatlib/api.hpp"
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
      d.add<sst::str>("hash") = a.hash;
      d.add<sst::str>("fileName") = a.fileName;
   }

   virtual void loadFromSst(sst::dict& d, asset& a)
   {
      throw 3.14;
   }
};

tcatExposeSingletonTypeAs(assetConverter,iAssetConverter);

} // namespace db
