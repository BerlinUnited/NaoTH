// 
// File:   DebugModify.h
// Author: Heinrich Mellmann
//
// Created on 19. march 2008, 21:51
//

#ifndef _DebugModify_H
#define _DebugModify_H

#include <Tools/DataStructures/Singleton.h>
#include <Tools/DataConversion.h>
#include <DebugCommunication/DebugCommandExecutor.h>

/**
  Usage: just write 
    MODIFY("myvalue", d); 
  where d is a numeric value which can be casted to double with static_cast 
  (can be local). Then this value can be modified using the RC.
 */
class DebugModify : public DebugCommandExecutor
{
public:
  DebugModify();
  virtual ~DebugModify();

  /**
  * 
  */
  class ModifyValue 
  {
  private:
    // TODO: find beter solution to hide private stuff
    friend class DebugModify;
    friend class std::map<std::string, ModifyValue>;

    ModifyValue() : value(0.0), modify(false) {}
    double value;
    bool modify;

  public:
    template<class T>
    void update(T& d)
    {
      if(modify)
        d = static_cast<T>(value);
      else
        value = static_cast<double>(d);
    }
  };//end clas ModifyValue

  virtual void executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::ostream &outstream);
  
  //@Depricated and shouldn't be used
  //since access using this method is very slow
  void updateValue(std::string name, double& d)
  {
    if(valueMap.find(name) != valueMap.end() && valueMap.find(name)->second.modify)
    {
      d = valueMap.find(name)->second.value;
    } else {
      valueMap[name].value = d;
    }
  }

  ModifyValue& getValueReference(std::string name, double& d)
  {
    // add value if not existing
    ModifyValue& modifyValue = valueMap[name];
    modifyValue.value = d;
      
    return modifyValue;
  }

private:
  std::map<std::string, ModifyValue> valueMap;
  
};

// MACROS //

#ifdef DEBUG
#define MODIFY(id, value) {static DebugModify::ModifyValue& _debug_modify_value_ = getDebugModify().getValueReference(id, value); _debug_modify_value_.update(value); }
#else
/* ((void)0) - that's a do-nothing statement */
#define MODIFY(id, value) ((void)0)
#endif // DEBUG


#endif  /* _DebugModify_H */

