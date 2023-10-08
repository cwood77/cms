#include "../tcatlib/api.hpp"
#include "api.hpp"
#include <map>
#include <stdexcept>

namespace db {

class tagFilter : public iTagFilter {
public:
   tagFilter() : m_pBase(NULL) {}

   virtual const std::list<asset>& provideAssets() const
   {
      // do this everytime in case base has changed
      const_cast<tagFilter&>(*this).recompute();
      return m_list;
   }

   virtual void configure(const std::string& value, iAssetProvider& base)
   {
      m_value = value;
      m_pBase = &base;
   }

private:
   void recompute()
   {
      m_list.clear();

      auto& assets = m_pBase->provideAssets();
      for(auto asset : assets)
         if(asset.tags.find(m_value)!=asset.tags.end())
            m_list.push_back(asset);
   }

   std::string m_value;
   iAssetProvider *m_pBase;
   std::list<asset> m_list;
};

tcatExposeTypeAs(tagFilter,iTagFilter);

} // namespace db
