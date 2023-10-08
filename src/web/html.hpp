#ifndef ___web_htmlWriter___
#define ___web_htmlWriter___

#include <list>
#include <map>
#include <ostream>
#include <sstream>
#include <string>

namespace web {
namespace html {

class iHtmlStream;

class node {
public:
   virtual ~node() {}
   virtual void write(iHtmlStream& o) const = 0;
};

class tagNode : public node {
public:
   virtual ~tagNode()
   {
      for(auto *pChild : m_children)
         delete pChild;
   }

   template<class T>
   T& addChild() { T *pN = new T(); addChild(*pN); return *pN; }

   std::map<std::string,std::string> attrs;

   virtual void write(iHtmlStream& o) const;

   virtual std::string getTag() const = 0;

private:
   void addChild(node& n) { m_children.push_back(&n); }

   std::list<node*> m_children;
};

class content : public node {
public:
   std::stringstream content;

   virtual void write(iHtmlStream& o) const;
};

#define cdwImplHtmlNode(__node__) \
class __node__ : public tagNode { \
protected: \
   virtual std::string getTag() const { return #__node__; } \
};

cdwImplHtmlNode(head)
cdwImplHtmlNode(title)
cdwImplHtmlNode(body)
cdwImplHtmlNode(h1)
cdwImplHtmlNode(br)
cdwImplHtmlNode(a)
cdwImplHtmlNode(h2)
cdwImplHtmlNode(table)
cdwImplHtmlNode(tr)
cdwImplHtmlNode(td)
cdwImplHtmlNode(img)

#undef cdwImplHtmlNode

class root : public tagNode {
protected:
   virtual std::string getTag() const { return "html"; }
};

class iHtmlStream {
public:
   virtual void writeTagOpen(const tagNode& t) = 0;
   virtual void writeTagClose(const tagNode& t) = 0;
   virtual void writeTagSelf(const tagNode& t) = 0;
   virtual void writeString(const std::string& t) = 0;
};

class htmlStream : public iHtmlStream {
public:
   explicit htmlStream(std::ostream& o) : m_o(o) {}

   virtual void writeTagOpen(const tagNode& t);
   virtual void writeTagClose(const tagNode& t);
   virtual void writeTagSelf(const tagNode& t);
   virtual void writeString(const std::string& t);

private:
   void writeAttrs(const tagNode& t);

   std::ostream& m_o;
};

} // namespace html
} // namespace web

#endif // ___web_htmlWriter___
