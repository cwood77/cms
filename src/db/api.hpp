#ifndef ___db_api___
#define ___db_api___

#include <list>
#include <set>
#include <string>

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
//    <HASH>.sst
//    <HASH>
//       <whatever>

// ---- objects that map basically to SST ----
#if 0
class usage {
public:
   std::string name;
   std::string desc;
   std::string folder;
};
#endif

class asset {
public:
   std::string guid; // maybe uneeded?
   std::string source;
   std::set<std::string> tags;
   std::string hash;
   std::string fileName;
};

#if 1
class usageRefs {
public:
   std::string lastUpdated;
   std::set<std::string> hashes;
};
#endif

// ---- ??? ----

class iAssetProvider {
public:
   virtual ~iAssetProvider() {}
   virtual const std::list<asset>& provideAssets() const = 0;
};

class iDb : public iAssetProvider {
public:
   virtual void publish(const asset& a, const std::wstring& fullAssetPath) = 0;
};

#if 0
// --extract <tag>
class iTagFilter : public iAssetProvider {
public:
   virtual void configure(const std::string& value, iAssetProvider& base) = 0;
};

// --extract <guid>
class iGuidFilter : public iAssetProvider {
public:
   virtual void configure(const std::string& value, iAssetProvider& base) = 0;
};

class iAssetExtractor {
public:
   virtual void configure(iAssetProvider& base) = 0;
   virtual void extract(const std::wstring& destRoot) = 0;
};
#endif

#if 1
// --refs
class iHashIndex {
public:
   virtual ~iHashIndex() {}
   virtual void configure(iAssetProvider& base) = 0;
   virtual bool hasAsset(const std::string& hash) const = 0;
};
#endif

} // namespace db

#endif // ___db_api___
