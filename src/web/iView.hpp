#ifndef ___web_iView___
#define ___web_iView___

namespace web {

class iView {
public:
   virtual ~iView() {}
   virtual std::string name() const = 0;
   virtual std::string linkTarget() const = 0;
};

} // namespace web

#endif // ___web_iView___
