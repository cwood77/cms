#ifndef ___db_iUsageRefsConverter___
#define ___db_iUsageRefsConverter___

namespace sst { class dict; }

namespace db {

class usageRefs;

class iUsageRefsConverter {
public:
   virtual ~iUsageRefsConverter() {}

   virtual void saveToSst(const usageRefs& v, sst::dict& d) = 0;
   virtual void loadFromSst(sst::dict& d, usageRefs& v) = 0;
};

} // namespace db

#endif // ___db_iUsageRefsConverter___
