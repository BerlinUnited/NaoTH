/**
* @file MotionEngine.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Declaration of class MotionFactory
*/

#ifndef _MotionFactory_h_
#define _MotionFactory_h_

#include "Motion/AbstractMotion.h"
#include <ModuleFramework/ModuleManager.h>
#include <ModuleFramework/Module.h>

class MotionFactory: protected ModuleManager
{
public:
  virtual ~MotionFactory(){}

  virtual Module* createMotion(const MotionRequest& motionRequest) = 0;


  class AbstractMotionCreator
  {
  public: 
    virtual AbstractMotion* createMotion() = 0;
    virtual void deleteMotion() = 0;
  };


  template<class T>
  class MotionCreator: public AbstractMotionCreator
  {
  private:
    ModuleCreator<T>* creator;

  public:
    MotionCreator()
      : creator(NULL)
    {
    }

    MotionCreator(ModuleCreator<T>* creator)
      : creator(creator)
    {
    }

    T* createMotion()
    {
      assert(creator != NULL);
      creator->setEnabled(true);
      return creator->getModuleT();
    }



    void deleteMotion()
    {
      assert(creator != NULL);
      creator->setEnabled(false);
    }
  };

  template<class T>
  MotionCreator<T> registerMotion(std::string name)
  {
    return MotionCreator<T>(registerModule<T>(name));
  }

};

#endif //_MotionFactory_h_
