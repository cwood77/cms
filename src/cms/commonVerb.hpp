#ifndef ___cms_commonVerb___
#define ___cms_commonVerb___

#include <functional>

namespace cms {

// standard boilerplate bits for most/all verbs:
// - load the config file
// - compose the services
// - etc
class commonVerb {
public:
   static void run(std::function<void()> f);
};

} // namespace cms

#endif // ___cms_commonVerb___
