#ifndef ___db_api___
#define ___db_api___

namespace db {

// tracks all used folders along with some basic info
// tracks all assets along with some basic info
// has a big dumping ground of assets by guid
//
// C:\ProgramData\cms
// global.sst
// usage.sst
// www
//    index.html
// assets
//    <GUID>.sst
//    <GUID>
//       <whatever>

// ---- objects that map basically to SST ----
class usage {
public:
   std::string name;
   std::string desc;
   std::string folder;
};

class asset {
public:
   std::string guid;
   std::string source;
   std::set<std::string> tags;
   std::string hash;
   std::string fileName;
};

class usageRefs {
public:
   std::string lastUpdated;
   std::list<std::string> guids;
};

// ---- used to link usages to assets ----
// this would have to be cached
class iAssetCatalog {
public:
   virtual const asset& demand(const std::string& hash) const = 0;
};

class iAssetCatalogBuilder {
public:
};

} // namespace db

#endif // ___db_api___
