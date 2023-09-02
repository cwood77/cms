#ifndef ___db_iAssetConverter___
#define ___db_iAssetConverter___

namespace sst { class dict; }

namespace db {

class asset;

class iAssetConverter {
public:
   virtual ~iAssetConverter() {}

   virtual void saveToSst(const asset& a, sst::dict& d) = 0;
   virtual void loadFromSst(sst::dict& d, asset& a) = 0;
};

} // namespace db

#endif // ___db_iAssetConverter___
