#include "../cmn/string.hpp"
#include "../file/api.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include "usageRefsConverter.hpp"

namespace db {

class usageRefsConverter : public iUsageRefsConverter {
public:
   virtual void saveToSst(const usageRefs& v, sst::dict& d)
   {
      d.add<sst::str>("guid") = v.guid;
      d.add<sst::str>("last-updated") = v.lastUpdated;
      auto& arr = d.add<sst::array>("hashes");
      for(auto& h : v.hashes)
         arr.append<sst::str>() = h;
   }

   virtual void loadFromSst(sst::dict& d, usageRefs& v)
   {
      throw 3.14;
   }
};

tcatExposeSingletonTypeAs(usageRefsConverter,iUsageRefsConverter);

} // namespace db
