#ifndef ___db_clientConverter___
#define ___db_clientConverter___

namespace sst { class dict; }

namespace db {

class client;
class clientRef;

class iClientConverter {
public:
   virtual ~iClientConverter() {}

   virtual void saveToSst(const client& v, sst::dict& d) = 0;
   virtual void loadFromSst(sst::dict& d, client& v) = 0;
};

class iClientRefConverter {
public:
   virtual ~iClientRefConverter() {}

   virtual void saveToSst(const clientRef& v, sst::dict& d) = 0;
   virtual void loadFromSst(sst::dict& d, clientRef& v) = 0;
};

} // namespace db

#endif // ___db_clientConverter___
