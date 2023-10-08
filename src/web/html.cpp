#include "html.hpp"
#include <stdexcept>

namespace web {
namespace html {

void tagNode::write(iHtmlStream& o) const
{
   if(m_children.size())
   {
      o.writeTagOpen(*this);
      for(auto *pC : m_children)
         pC->write(o);
      o.writeTagClose(*this);
   }
   else
      o.writeTagSelf(*this);
}

void content::write(iHtmlStream& o) const
{
   o.writeString(content.str());
}

void htmlStream::writeTagOpen(const tagNode& t)
{
   m_o << "<" << t.getTag();
   writeAttrs(t);
   m_o << ">";
}

void htmlStream::writeTagClose(const tagNode& t)
{
   m_o << "</" << t.getTag() << ">" << std::endl;
}

void htmlStream::writeTagSelf(const tagNode& t)
{
   m_o << "<" << t.getTag();
   writeAttrs(t);
   m_o << "/>" << std::endl;
}

void htmlStream::writeString(const std::string& t)
{
   m_o << t;
}

void htmlStream::writeAttrs(const tagNode& t)
{
   for(auto it=t.attrs.begin();it!=t.attrs.end();++it)
      m_o << " " << it->first << "=\"" << it->second << "\"";
}

} // namespace html
} // namespace web
