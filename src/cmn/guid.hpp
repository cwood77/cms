#ifndef ___cmn_guid___
#define ___cmn_guid___

#define WIN32_LEAN_AND_MEAN
#include <iomanip>
#include <objbase.h>
#include <sstream>

namespace cmn {

// GUIDs are handled as strings
class guid {
public:
   static std::string create()
   {
      GUID g;
      auto hr = ::CoCreateGuid(&g);
      if(!SUCCEEDED(hr))
         throw std::runtime_error("filed to allocate guid");

      std::stringstream stream;
      stream
         << std::hex << std::setfill('0') << std::nouppercase
         << std::setw(8) << g.Data1
         << "-"
         << std::setw(4) << g.Data2
         << "-"
         << std::setw(4) << g.Data3
         << "-"
         << std::setw(2) << (unsigned int)g.Data4[0]
         << std::setw(2) << (unsigned int)g.Data4[1]
         << std::setw(2) << (unsigned int)g.Data4[2]
         << std::setw(2) << (unsigned int)g.Data4[3]
      ;

      return stream.str();
   }
};

} // namespace cmn

#endif // ___cmn_guid___
