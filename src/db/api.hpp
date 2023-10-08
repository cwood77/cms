#ifndef ___db_api___
#define ___db_api___

#include <list>
#include <set>
#include <string>

namespace db {

class iAssetFileTypeInfo {
public:
   virtual bool isWebViewable() const = 0;
};

class iAssetFileTypeExpert {
public:
   virtual ~iAssetFileTypeExpert() {}
   virtual iAssetFileTypeInfo *fetch(const std::wstring& ext) = 0;
};

// ---- objects that map to SST ----

class client {
public:
   std::string guid;
   std::string name;
   std::string lastKnownFolder;
};

class clientRef {
public:
   std::string guid;
};

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
   std::string legal;
   std::string hash;
   std::string fileName;
   std::string thumbnailExt;
};

class usageRefs {
public:
   std::string guid; // of client
   std::string lastUpdated;
   std::set<std::string> hashes;
};

// ---- ??? ----

class iAssetProvider {
public:
   virtual ~iAssetProvider() {}
   virtual const std::list<asset>& provideAssets() const = 0;
};

class iDb : public iAssetProvider {
public:
   virtual void saveClient(const client& c) = 0;
   virtual std::list<client> listClients() const = 0;

   virtual client getReferencedClient(const std::wstring& refPath) const = 0;

   virtual void publish(const asset& a, const std::wstring& fullAssetPath, const std::wstring& fullThumbnailPath) = 0;
   virtual void erase(const asset& a) = 0;

   virtual void saveRefs(const usageRefs& r) = 0;

   virtual void commit() = 0;
};

// --extract <tag>
class iTagFilter : public iAssetProvider {
public:
   virtual void configure(const std::string& value, iAssetProvider& base) = 0;
};

#if 0
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

// --refs
class iHashIndex {
public:
   virtual ~iHashIndex() {}
   virtual void configure(iAssetProvider& base) = 0;
   virtual bool hasAsset(const std::string& hash) const = 0;
};

class iDbObserver {
public:
   virtual ~iDbObserver() {}
   virtual void onSave(iDb& db) = 0;
};

} // namespace db

#endif // ___db_api___
