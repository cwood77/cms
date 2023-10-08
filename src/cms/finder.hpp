#ifndef ___cms_finder___
#define ___cms_finder___

#include "../db/api.hpp"
#include <functional>
#include <map>

namespace console { class iLog; }

namespace cms {

class zipHandler {
public:
   zipHandler();

   bool noteFile(const std::wstring& fullPath, const std::wstring& fileName);
   bool shouldDescendIntoFolder(const std::wstring& fullPath);

   void processDeferredAdds(std::function<void(const std::wstring&, const std::wstring&, const std::wstring&)> f);

private:
   std::map<std::wstring,std::wstring> m_foldersToHandle;
   std::map<std::wstring,std::wstring> m_fileNames;
   console::iLog *m_pLog;
};

class assetFinder {
public:
   class iFileInfo {
   public:
      virtual const std::wstring& fileName() const = 0;
      virtual const std::wstring& ext() const = 0;
      virtual const std::wstring& fullFilePath() const = 0;
      virtual const std::wstring& thumbnailFullFilePath() const = 0;
      virtual const std::string& hash() const = 0;
   };

   static void find(const std::wstring& path, std::function<void(iFileInfo&)> f, bool chatty = true);

private:
   assetFinder(std::function<void(iFileInfo&)> f, bool chatty);

   void find(const std::wstring& path);
   void considerFile(const std::wstring& fullPath, const std::wstring& fileName);

   const bool m_chatty;
   tcat::typePtr<db::iAssetFileTypeExpert> m_pAExpert;
   zipHandler m_zip;
   std::function<void(iFileInfo&)> m_f;
   console::iLog *m_pLog;
};

} // namespace cms

#endif // ___cms_finder___
