/**
* @file KeyFrameMotionFactory.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class KeyFrameMotionFactory
*/

#ifndef _KeyFrameMotionFactory_h_
#define _KeyFrameMotionFactory_h_

#include "Tools/KeyFrameMotion.h"

#include "Representations/Motion/Request/MotionRequest.h"
#include "Motion/Engine/MotionFactory.h"

#include <string>

/**
* representation holding the loaded motion nets
*/
class MotionNets
{
private:
  typedef std::map<std::string, MotionNet> MotionNetsRegistry;
  MotionNetsRegistry registry;

public:
  MotionNet& operator[] (std::string name){ return registry[name]; }
  bool exist(std::string name) const { return registry.find(name) != registry.end(); }
  
  const MotionNet* get(std::string name) const 
  { 
    MotionNetsRegistry::const_iterator iter = registry.find(name); 
    if(iter != registry.end())
      return &(iter->second);
    else
      return NULL;
  }
};


#include <ModuleFramework/Module.h>

BEGIN_DECLARE_MODULE(KeyFrameMotionFactory)
  PROVIDE(MotionNets)
END_DECLARE_MODULE(KeyFrameMotionFactory)


class KeyFrameMotionFactory: public MotionFactory, private KeyFrameMotionFactoryBase //TODO: , public DebugCommandExecutor
{
public:

  KeyFrameMotionFactory();
  ~KeyFrameMotionFactory();

  Module* createMotion(const MotionRequest& motionRequest);

  void execute(){} // dummy

  /*virtual void executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::stringstream &outstream);*/

private:
  ModuleCreator<KeyFrameMotion>* keyFrameMotionCreator;

  // loading motion-nets
  void loadMotionNetFromFile(const std::string& fileName, MotionNet& motionNet) const;
  bool loadMotionNet(const std::string& fileName, const std::string& motionNetName);
  void loadAvailableMotionNets(std::string directoryName);
};

#endif //_KeyFrameMotionFactory_h_
