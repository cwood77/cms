#include "../cmn/string.hpp"
#include "../file/api.hpp"
#include "../tcatlib/api.hpp"
#include "api.hpp"
#include "clientConverter.hpp"

namespace db {

class clientConverter : public iClientConverter {
public:
   virtual void saveToSst(const client& v, sst::dict& d)
   {
      d.add<sst::str>("guid") = v.guid;
      d.add<sst::str>("name") = v.name;
      d.add<sst::str>("lastKnownFolder") = v.lastKnownFolder;
   }

   virtual void loadFromSst(sst::dict& d, client& v)
   {
      v.guid = d["guid"].as<sst::str>().get();
      v.name = d["name"].as<sst::str>().get();
      v.lastKnownFolder = d["lastKnownFolder"].as<sst::str>().get();
   }
};

tcatExposeSingletonTypeAs(clientConverter,iClientConverter);

class clientRefConverter : public iClientRefConverter {
public:
   virtual void saveToSst(const clientRef& v, sst::dict& d)
   {
      d.add<sst::str>("guid") = v.guid;
   }

   virtual void loadFromSst(sst::dict& d, clientRef& v)
   {
      v.guid = d["guid"].as<sst::str>().get();
   }
};

tcatExposeSingletonTypeAs(clientRefConverter,iClientRefConverter);

} // namespace db

