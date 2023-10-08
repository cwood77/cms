#ifndef ___cms_finder___
#define ___cms_finder___

#include "../db/api.hpp"
#include <functional>
#include <set>

namespace console { class iLog; }

namespace cms {

class zipHandler {
public:
   zipHandler();

   void noteFile(const std::wstring& fullPath, const std::wstring& fileName);
   bool shouldDescendIntoFolder(const std::wstring& fullPath) const;

private:
   std::set<std::wstring> m_foldersToIgnore;
   console::iLog *m_pLog;
};

class assetFinder {
public:
   class iFileInfo {
   public:
      virtual const std::wstring& fileName() const = 0;
      virtual const std::wstring& ext() const = 0;
      virtual const std::wstring& fullFilePath() const = 0;
      virtual const std::string& hash() const = 0;
   };

   static void find(const std::wstring& path, std::function<void(iFileInfo&)> f);

private:
   explicit assetFinder(std::function<void(iFileInfo&)> f);

   void find(const std::wstring& path);
   void considerFile(const std::wstring& fullPath, const std::wstring& fileName);

   tcat::typePtr<db::iAssetFileTypeExpert> m_pAExpert;
   zipHandler m_zip;
   std::function<void(iFileInfo&)> m_f;
   console::iLog *m_pLog;
};

} // namespace cms

#endif // ___cms_finder___
