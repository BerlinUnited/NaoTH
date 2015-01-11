
#include "Module.h"

void Module::registerProvide(const RegistrationInterfaceMap& rr_map)
{
  RegistrationInterfaceMap::const_iterator iter = rr_map.begin();

  for(;iter != rr_map.end(); ++iter) 
  {
    // init the actual dependency to te black board
    Representation& representation = (*iter).second->registerAtBlackBoard(getBlackBoard());
    providedMap[iter->first] = &representation;

    // register this module at the representation
    representation.registerProvide(*this);
  }//end for
}//end registerProvide
  

void Module::registerRequire(const RegistrationInterfaceMap& rr_map)
{
  RegistrationInterfaceMap::const_iterator iter = rr_map.begin();

  for(;iter != rr_map.end(); ++iter)
  {
    // init the actual dependency to te black board
    Representation& representation = (*iter).second->registerAtBlackBoard(getBlackBoard());
    requiredMap[iter->first] = &representation;

    // register this module at the representation
    representation.registerRequire(*this);
  }//end for
}//end registerRequire


void Module::unregister(RepresentationMap& r_map)
{
  RepresentationMap::iterator iter = r_map.begin();
  for(;iter != r_map.end(); ++iter) {
    iter->second->unregisterRequire(*this);
  }
  r_map.clear();
}//end unregister

std::ostream& operator <<(std::ostream &stream, const Module& module)
{
  stream << "[*" << module.getName() << "*]" << std::endl;
  RepresentationMap::const_iterator i = module.getRequire().begin();
  for(;i != module.getRequire().end(); ++i)
  {
    stream << " > " << i->first << std::endl;
  }//end for

  i = module.getProvide().begin();
  for(;i != module.getProvide().end(); ++i)
  {
    stream << " < " << i->first << std::endl;
  }//end for

  return stream;
}// end <<

std::string get_sub_core_module_path(std::string fullpath)
{
  unsigned p = static_cast<unsigned int> (fullpath.find("core"));
  if (p < fullpath.size()-5) {
    return fullpath.substr(p+5); // size of "core/"
  } else {
    return fullpath;
  }
}
