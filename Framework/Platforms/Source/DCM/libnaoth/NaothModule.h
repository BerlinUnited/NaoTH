/**
* @file NaothModule.cpp
*
* @author <a href="mailto:welter@informatik.hu-berlin.de">Oliver Welter</a>
* Deklaration of NaothModule
*/

#ifndef _NAOTHMODULE_H
#define	_NAOTHMODULE_H

#include "Motion.h"
#include "NaoMotionController.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Tools/NaoTime.h"

using namespace AL;
using namespace std;

namespace naoth
{

class NaothModule : public ALModule
{
  private:
    //std::string controllerName;

    ALPtr<ALBroker> pBroker;

    NaoMotionController* theContorller;
    Motion* theMotion;
    
    class DummyCognition : public naoth::Callable
    {
    public:
      virtual void call() {};
      void init(PlatformInterfaceBase& platformInterface) {};
    } theCognition;
  
  public:
    
    /**
     * Default Constructor.
     */
    NaothModule(ALPtr<ALBroker> pBroker, const std::string& pName );
    
    /**
     * Destructor.
     */
    virtual ~NaothModule();
    
    /**
     * version
     * @return The version number of ALLeds
     */
    std::string version();
    
    bool innerTest();
    
    virtual void init();
    virtual void exit( );

    void motionCallbackPre();
    void motionCallbackPost();

private:

};

} // end namespace naoth

#endif	/* _NAOTHMODULE_H */
