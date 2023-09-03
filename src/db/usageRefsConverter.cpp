#include "../cmn/string.hpp"
#include "../file/api.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include "usageRefsConverter.hpp"
#include <sstream>

namespace db {

class usageRefsConverter : public iUsageRefsConverter {
public:
   virtual void saveToSst(const usageRefs& a, sst::dict& d)
   {
      d.add<sst::str>("last-updated") = a.lastUpdated;
      auto& arr = d.add<sst::array>("hashes");
      for(auto& h : a.hashes)
         arr.append<sst::str>() = h;
   }

   virtual void loadFromSst(sst::dict& d, usageRefs& a)
   {
      throw 3.14;
   }
};

tcatExposeSingletonTypeAs(usageRefsConverter,iUsageRefsConverter);

} // namespace db
