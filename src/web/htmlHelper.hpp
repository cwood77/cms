#ifndef ___web_htmlWriterHelper___
#define ___web_htmlWriterHelper___

namespace web {
class iView;
namespace html { class root; }
namespace html { class tagNode; }

class htmlWriterHelper {
public:
   static html::tagNode& boilerplate(web::html::root& r, iView& thisView);
};

} // namespace web

#endif // ___web_htmlWriterHelper___
