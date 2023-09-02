#include "../tcatlib/api.hpp"
#include "api.hpp"
#include <map>
#include <stdexcept>

namespace db {

class hashIndex : public iHashIndex {
public:
   virtual void configure(iAssetProvider& base)
   {
      m_pBase = &base;
      categorize();
   }

   virtual bool hasAsset(const std::string& hash) const
   {
      return m_dict.find(hash)!=m_dict.end();
   }

private:
   void categorize()
   {
      auto& l = m_pBase->provideAssets();
      for(auto& a : l)
         categorize(a);
   }

   void categorize(const asset& a)
   {
      const asset*& pAsset = m_dict[a.hash];
      if(pAsset)
         throw std::runtime_error("asset with duplicate hash");
      pAsset = &a;
   }

   iAssetProvider *m_pBase;
   std::map<std::string,const asset*> m_dict;
};

tcatExposeSingletonTypeAs(hashIndex,iHashIndex);

} // namespace db
