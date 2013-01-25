/**
* @file ModuleCreator.h
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
*/

#ifndef _ModuleCreator_h_
#define _ModuleCreator_h_

#include "Tools/DataStructures/Printable.h"
#include "Module.h"


/**
 * AbstractModuleCreator is an interface.
 * It is used to create lists of modules.
 * Additionally it provides functionality to enable/disable the module
 */
class AbstractModuleCreator: public Printable
{
public:
  virtual std::string moduleClassName() const = 0;
  virtual void setEnabled(bool value) = 0;
  virtual bool isEnabled() const = 0;
  virtual void execute() = 0;
  virtual Module* getModule() const = 0;
  virtual ~AbstractModuleCreator() {}

  virtual const RegistrationInterfaceMap& staticProvided() const = 0;
  virtual const RegistrationInterfaceMap& staticRequired() const = 0;
};

/**
 * ModuleInstance is needed to instantiate the 
 * BlackBoardInterface of the class V (if it has one)
 * with a blackboard.
 * We assume, that the class V already inherites from BlackBoardInterface.
 * Thereby 'virtual' inheritence is esential (!).
 * 
 * (in fact, what we are doing is to extend the default constructor of the class V
 *  by providing a pointer to a blackboard instance, i.e., we call another 
 *  constructor of the BlackBoardInterface)
 */
template<class V>
class ModuleInstance: virtual public BlackBoardInterface, virtual public V
{
public:
  ModuleInstance(BlackBoard& theBlackBoard)
    : BlackBoardInterface(&theBlackBoard)
  {}
};

/**
 * ModuleCreator implements the AbstractModuleCreator.
 * A module is deleted if it is disabled and created new if it is enabled.
 */
template<class M>
class ModuleCreator : public AbstractModuleCreator
{
private:
  BlackBoard& theBlackBoard;
  M* theInstance;

  // cannot be copied
  ModuleCreator& operator=( const ModuleCreator& ) {}

public:

  ModuleCreator(BlackBoard& theBlackBoard, bool enabled = false)
    : theBlackBoard(theBlackBoard),
      theInstance(NULL)
  {
    setEnabled(enabled);
  }

  virtual ~ModuleCreator() {
    delete theInstance;
  }


  bool isEnabled() const {
    return theInstance != NULL;
  }//end isEnabled


  void setEnabled(bool value)
  {
    if(value) {
      if( theInstance == NULL ) {
        theInstance = new ModuleInstance<M>(theBlackBoard);
      }
    } else {
      //NOTE: it is safe to delete NULL
      delete theInstance;
      theInstance = NULL;
    }
  }//end setEnabled


  void execute()
  {
    if( theInstance != NULL ) {
      theInstance->execute();
    }//end if
  }//end execute


  Module* getModule() const
  {
    ASSERT(isEnabled());
    // ACHTUNG: 
    // we have to use the unsafe cast because some modules may privatly 
    // derive from Module and make a type cast inaccesible
    return (Module*)(theInstance);
  }//end getModule


  M* getModuleT() const {
    ASSERT(isEnabled());
    return theInstance;
  }//end getModule


  std::string moduleClassName() const {
    return typeid(M).name();
  }


  const RegistrationInterfaceMap& staticProvided() const {
    return IF<M>::getProvide();
  }

  const RegistrationInterfaceMap& staticRequired() const {
    return IF<M>::getRequire();
  }

  void print(std::ostream& stream) const
  {
    if(isEnabled()) {
      stream << *getModule();
      return;
    }

    stream << "[ " << moduleClassName() << " ]" << std::endl;
    RegistrationInterfaceMap::const_iterator i = staticRequired().begin();
    for(;i != staticRequired().end(); ++i) {
      stream << " > " << i->first << std::endl;
    }//end for

    i = staticProvided().begin();
    for(;i != staticProvided().end(); i++) {
      stream << " < " << i->first << std::endl;
    }//end for
  }//end print
};

#endif //_ModuleCreator_h_
