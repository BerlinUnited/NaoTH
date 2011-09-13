// 
// File:   DebugModify.h
// Author: Heinrich Mellmann
//
// Created on 19. März 2008, 21:51
//

#ifndef _DebugModify_H
#define  _DebugModify_H

#include <Tools/DataStructures/Singleton.h>
#include <Tools/DataConversion.h>
#include <DebugCommunication/DebugCommandExecutor.h>

/**
  Usage: just write 
    MODIFY("myvalue", d); 
  where d is a double value (can be local). Then this value can be modified using the RC:
 */
class DebugModify : public naoth::Singleton<DebugModify>, public DebugCommandExecutor
{
protected:
  friend class naoth::Singleton<DebugModify>;
  DebugModify();
  virtual ~DebugModify();

public:

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
    void update(double& d)
    {
      if(modify)
        d = value;
      else
        value = d;
    }
  };

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
    }else
    {
      valueMap[name].value = d;
    }
  }//end updateValue

  ModifyValue& getValueReference(std::string name, double& d)
  {
    // add value if not existing
    ModifyValue& modifyValue = valueMap[name];
    modifyValue.value = d;
      
    return modifyValue;
  }//end getValueReference

private:
  std::map<std::string, ModifyValue> valueMap;
  
};

// MACROS //

#ifdef DEBUG
#define MODIFY(id, value) {static DebugModify::ModifyValue& _debug_modify_value_ = DebugModify::getInstance().getValueReference(id, value); _debug_modify_value_.update(value); }
#else
/* ((void)0) - that's a do-nothing statement */
#define MODIFY(id, value) ((void)0)
#endif // DEBUG


#endif  /* _DebugModify_H */

