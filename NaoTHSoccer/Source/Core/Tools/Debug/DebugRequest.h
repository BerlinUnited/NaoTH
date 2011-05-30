// 
// File:   DebugRequest.h
// Author: thomas
//
// Created on 19. März 2008, 21:51
//

#ifndef _DEBUGREQUEST_H
#define	_DEBUGREQUEST_H

#include <cstring>
#include <map>

#include <Tools/DataStructures/Singleton.h>
#include <DebugCommunication/DebugCommandExecutor.h>

class DebugRequest : public naoth::Singleton<DebugRequest>, public DebugCommandExecutor
{
protected:
  friend class naoth::Singleton<DebugRequest>;
  DebugRequest();
  virtual ~DebugRequest();

public:

  /** Register debug request. Will do nothing if already known. */
  void registerRequest(const std::string& name, const std::string& description, bool defaultValue);
  
  //@Depricated and shouldn't be used
  //since access using this method is very slow
  /** True if debug requested is activated. False if not. */
  bool isActive(const std::string& name) const;

  /** 
   * Return a reference to the value belonging to the debug request name.
   * It is used for fast access to the values. (see DEBUG_REQUEST)
   */
  const bool& getValueReference(const std::string& name) const;
  
  virtual void executeDebugCommand(
    const std::string& command, const std::map<std::string,std::string>& arguments,
    std::ostream &outstream);
  
private:
  std::map<std::string, bool> requestMap;
  std::map<std::string, std::string> descriptionMap;
};

// MACROS //

#ifdef DEBUG
  /** Execute the code depending wether the request with this name is active ,fast version*/
  #define DEBUG_REQUEST(name, code) { static const bool& _debug_request_is_active_ = DebugRequest::getInstance().getValueReference(name); if(_debug_request_is_active_){code}} ((void)0)
  /** Execute the code depending wether the request with this name is active */
  #define DEBUG_REQUEST_SLOW(name, code) { if(DebugRequest::getInstance().getValueReference(name)){code}} ((void)0)
  /** Execute the code depending wether the request with this name is active, slow version (for carefull use in loops)*/
  #define DEBUG_REQUEST_IF_ACTIVE(name, code) { static const bool& _debug_request_is_active_ = DebugRequest::getInstance().getValueReference(name); if(_debug_request_is_active_){code}} ((void)0)
  /** Register debug request. Will do nothing if already known. */
  #define DEBUG_REQUEST_REGISTER(name, description, isActiveByDefault) {DebugRequest::getInstance().registerRequest(name, description, isActiveByDefault);} ((void)0)
  /** Execute the code when the request changes from active to deactive */
  #define DEBUG_REQUEST_ON_DEACTIVE(name, code) { static const bool& _debug_request_is_active_ = DebugRequest::getInstance().getValueReference(name); static bool _old_debug_request_is_active_ = false; if(_debug_request_is_active_){_old_debug_request_is_active_=true;}else if (_old_debug_request_is_active_) {_old_debug_request_is_active_=false; code}} ((void)0)
  /** Will be executed like a debug request in DEBUG or always in  RELEASE*/
  #define DEBUG_REQUEST_OR_RELEASE(name, code) DEBUG_REQUEST(name, code)
#else
  /* ((void)0) - that's a do-nothing statement */
  #define DEBUG_REQUEST(name, code) ((void)0)
  #define DEBUG_REQUEST_SLOW(name, code) ((void)0)
  #define DEBUG_REQUEST_REGISTER(name, description, isActiveByDefault) ((void)0)
  #define DEBUG_REQUEST_ON_DEACTIVE(name, code) ((void)0)
  #define DEBUG_REQUEST_OR_RELEASE(name, code) {code} ((void)0)
#endif // DEBUG


#endif	/* _DEBUGREQUEST_H */

