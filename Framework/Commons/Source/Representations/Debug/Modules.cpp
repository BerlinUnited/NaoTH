/* 
 * File:   Modules.cpp
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#include "Modules.h"


#include "Messages/Messages.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

void naoth::Serializer<Modules>::serialize(const Modules& object, std::ostream& stream)
{
  naothmessages::Modules msg;
  
  Modules::ModuleCreatorMap::const_iterator iterModule;
  for (iterModule = object.getModules().begin(); iterModule != object.getModules().end(); ++iterModule)
  {
    naothmessages::Module* m = msg.add_modules();

    // get the module holder
    AbstractModuleCreator* moduleCreator = iterModule->second;//getModule(*iterModule);
    ASSERT(moduleCreator != NULL); // should never happen

    // module name
    m->set_name(iterModule->first);
    m->set_path(moduleCreator->modulePath());
    m->set_active(moduleCreator->isEnabled());
      
    //if(moduleCreator->isEnabled())
    {
      //Module* module = moduleCreator->getModule();
      //ASSERT(module != NULL); // should never happen

      for(RegistrationInterfaceMap::const_reference r: moduleCreator->staticRequired()) {
        m->add_usedrepresentations(r.second->getName());
      }

      for (RegistrationInterfaceMap::const_reference r: moduleCreator->staticProvided()) {
        m->add_providedrepresentations(r.second->getName());
      }
    }//end if
  }//end for iterModule
    
  google::protobuf::io::OstreamOutputStream buf(&stream);
  msg.SerializeToZeroCopyStream(&buf);
}

void naoth::Serializer<Modules>::deserialize(std::istream& stream, Modules& object)
{
  naothmessages::Modules msg;
  google::protobuf::io::IstreamInputStream buf(&stream);
  msg.ParseFromZeroCopyStream(&buf);

  for(int i = 0; i < msg.modules_size(); ++i) {
    AbstractModuleCreator* module = object.getModule(msg.modules(i).name());
    if(module != NULL) {
      module->setEnabled(msg.modules(i).active());
    }
  }
}
