#ifndef ___web_htmlWriterHelper___
#define ___web_htmlWriterHelper___

namespace db { class asset; }
namespace web {
class iView;
namespace html { class root; }
namespace html { class tagNode; }

class htmlWriterHelper {
public:
   static html::tagNode& boilerplate(web::html::root& r, iView& thisView);

   static void assetTable(std::list<db::asset>& l, html::tagNode& b);
};

} // namespace web

#endif // ___web_htmlWriterHelper___
