#include "../tcatlib/api.hpp"
#include "api.hpp"
#include <map>

namespace db {
namespace {

class assetFileTypeInfo : public iAssetFileTypeInfo {
public:
   explicit assetFileTypeInfo(bool wv = false) : m_wv(wv) {}

   virtual bool isWebViewable() const { return m_wv; }

private:
   bool m_wv;
};

class assetFileTypeExpert : public iAssetFileTypeExpert {
public:
   assetFileTypeExpert()
   {
      m_info[L"avif"] = assetFileTypeInfo(true);
      m_info[L"jfif"] = assetFileTypeInfo(true);
      m_info[L"jpg"]  = assetFileTypeInfo(true);
      m_info[L"png"]  = assetFileTypeInfo(true);
      m_info[L"webp"] = assetFileTypeInfo(true);
      m_info[L"zip"]  = assetFileTypeInfo(false);
   }

   virtual iAssetFileTypeInfo *fetch(const std::wstring& ext)
   {
      auto it = m_info.find(ext);
      if(it == m_info.end())
         return NULL;
      return &it->second;
   }

private:
   std::map<std::wstring,assetFileTypeInfo> m_info;
};

tcatExposeSingletonTypeAs(assetFileTypeExpert,iAssetFileTypeExpert);

} // anonymous namespace
} // namespace db
