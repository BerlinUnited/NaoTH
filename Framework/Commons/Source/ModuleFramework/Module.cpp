
#include "Module.h"

void Module::unregisterRequire()
{
  for(const auto& r : requiredMap) {
    r.second->unregisterRequire(*this);
  }
  requiredMap.clear();
}

void Module::unregisterProvide()
{
  for(const auto& r : providedMap) {
    r.second->unregisterProvide(*this);
  }
  providedMap.clear();
}

std::ostream& operator <<(std::ostream &stream, const Module& module)
{
  stream << "[*" << module.getName() << "*]" << std::endl;
  
  RepresentationMap::const_iterator i = module.getRequire().begin();
  for(;i != module.getRequire().end(); ++i) {
    stream << " > " << i->first << std::endl;
  }

  i = module.getProvide().begin();
  for(;i != module.getProvide().end(); ++i) {
    stream << " < " << i->first << std::endl;
  }

  return stream;
}// end <<
