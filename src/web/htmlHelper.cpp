#include "../cmn/string.hpp"
#include "../db/api.hpp"
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

std::string htmlWriterHelper::assetTable(std::list<db::asset>& l, html::tagNode& b)
{
   std::string overallLegal = "legal";
   tcat::typePtr<db::iAssetFileTypeExpert> aFTEx;

   auto& t = b.addChild<html::table>();
   t.attrs["border"]="1";
   {
      auto& r = t.addChild<html::tr>();
      r.addChild<html::td>()
         .addChild<html::content>().content << "thumbnail";
      r.addChild<html::td>()
         .addChild<html::content>().content << "filename";
      r.addChild<html::td>()
         .addChild<html::content>().content << "source";
      r.addChild<html::td>()
         .addChild<html::content>().content << "hash";
      r.addChild<html::td>()
         .addChild<html::content>().content << "free use?";
   }

   std::map<std::string,db::asset*> sorted;
   for(auto& a : l)
      sorted[a.fileName] = &a;

   for(auto it=sorted.begin();it!=sorted.end();++it)
   {
      auto& r = t.addChild<html::tr>();
      auto& tn = r.addChild<html::td>();
      {
         auto ext = cmn::lower(cmn::splitExt(cmn::widen(it->first)));
         auto *pFT = aFTEx->fetch(ext);
         if(pFT->isWebViewable())
         {
            auto& i = tn.addChild<html::img>();
            i.attrs["src"] = "../assets/" + it->second->hash + "." + cmn::narrow(ext);
            i.attrs["style"] = "max-height: 100px; max-width: 100px;";
         }
         else if(!it->second->thumbnailExt.empty())
         {
            // try a thumbnail instead
            auto& i = tn.addChild<html::img>();
            i.attrs["src"] = "../assets/" + it->second->hash + "-tn." + it->second->thumbnailExt;
            i.attrs["style"] = "max-height: 100px; max-width: 100px;";
         }
      }
      r.addChild<html::td>()
         .addChild<html::content>().content << it->first;
      r.addChild<html::td>()
         .addChild<html::content>().content << it->second->source;
      r.addChild<html::td>()
         .addChild<html::content>().content << it->second->hash;
      r.addChild<html::td>()
         .addChild<html::content>().content << it->second->legal;

      if(it->second->legal == "illegal" && overallLegal == "legal")
         overallLegal = it->second->legal;
      else if(it->second->legal == "unknown")
         overallLegal = it->second->legal;
   }

   return overallLegal;
}

} // namespace web
