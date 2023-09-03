#ifndef ___db_iUsageRefsConverter___
#define ___db_iUsageRefsConverter___

namespace sst { class dict; }

namespace db {

class usageRefs;

class iUsageRefsConverter {
public:
   virtual ~iUsageRefsConverter() {}

   virtual void saveToSst(const usageRefs& a, sst::dict& d) = 0;
   virtual void loadFromSst(sst::dict& d, usageRefs& a) = 0;
};

} // namespace db

#endif // ___db_iUsageRefsConverter___
