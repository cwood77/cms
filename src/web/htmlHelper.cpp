#include "../tcatlib/api.hpp"
#include "html.hpp"
#include "htmlHelper.hpp"
#include "iView.hpp"
#include <map>

namespace web {

html::tagNode& htmlWriterHelper::boilerplate(web::html::root& r, iView& thisView)
{
   r.addChild<html::head>()
      .addChild<html::title>()
         .addChild<html::content>()
            .content << thisView.name();

   auto& b = r.addChild<html::body>();
   b.addChild<html::h1>()
      .addChild<html::content>()
         .content << thisView.name();

   {
      std::map<std::string,iView*> sorted;
      tcat::typeSet<iView> views;
      for(size_t i=0;i<views.size();i++)
         sorted[views[i]->name()] = &*views[i];

      auto *pC = &b.addChild<html::content>();
      pC->content << "[ ";
      for(auto it=sorted.begin();it!=sorted.end();++it)
      {
         if(it->first == thisView.name())
            pC->content << it->first << " ";
         else
         {
            auto& a = b.addChild<html::a>();
            a.attrs["href"] = it->second->linkTarget();
            a.addChild<html::content>().content << it->first << " ";
            pC = &b.addChild<html::content>();
         }
      }
      pC->content << "]";

      b.addChild<html::br>();
      b.addChild<html::br>();
   }

   return b;
}

} // namespace web
