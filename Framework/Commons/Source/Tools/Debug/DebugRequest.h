// 
// File:   DebugRequest.h
// Author: thomas
//
// Created on 19. march 2008, 21:51
//

#ifndef _DEBUGREQUEST_H
#define _DEBUGREQUEST_H

#include <cstring>
#include <map>

#include <DebugCommunication/DebugCommandExecutor.h>
#include "Tools/DataStructures/Serializer.h"

class DebugRequest : public DebugCommandExecutor
{
public:
  class Request
  {
  public:
    Request() : value(false) {}
    Request(bool value) : value(value) {}
    Request(bool value, const std::string& description) : value(value), description(description) {}
    bool value;
    std::string description;
  };

  typedef std::map<std::string, Request> RequestMap;

public:
  DebugRequest();
  virtual ~DebugRequest();


  /** Register debug request. Will do nothing if already known. */
  const bool& registerRequest(const std::string& name, const std::string& description, bool defaultValue);
  
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
  
  const RequestMap& getRequestMap() const {
    return requestMap;
  }

  RequestMap& getRequestMap() {
    return requestMap;
  }

private:
  RequestMap requestMap;
};

std::string get_sub_core_path(std::string fullpath);

namespace naoth
{
template<>
class Serializer<DebugRequest>
{
public:
  static void serialize(const DebugRequest& object, std::ostream& stream);
  static void deserialize(std::istream& stream, DebugRequest& object);
};
}


// MACROS //

#ifdef DEBUG
  #define STRINGIZE_NX(A) #A
  #define STRINGIZE(A) STRINGIZE_NX(A)
  #define MAKE_DESCTIPTION(description) std::string(description).append("\n").append(get_sub_core_path(__FILE__)).append(":").append(STRINGIZE(__LINE__))

  /** Register debug request. Will do nothing if already known. */
  #define DEBUG_REQUEST_REGISTER(name, description, isActiveByDefault) {getDebugRequest().registerRequest(name, MAKE_DESCTIPTION(description), isActiveByDefault);} ((void)0)
  /** Execute the code depending wether the request with this name is active, fast version */
  #define DEBUG_REQUEST(name, code) { static const bool& _debug_request_is_active_ = getDebugRequest().getValueReference(name); if(_debug_request_is_active_){code}} ((void)0)
  /** Execute the code depending wether the request with this name is active */
  #define DEBUG_REQUEST_GENERIC(name, code) { if(getDebugRequest().getValueReference(name)){code}} ((void)0)
  /** Execute the code depending wether the request with this name is active, slow version (for carefull use in loops)*/
  #define DEBUG_REQUEST_IF_ACTIVE(name, code) { static const bool& _debug_request_is_active_ = getDebugRequest().getValueReference(name); if(_debug_request_is_active_){code}} ((void)0)
  /** Execute the code when the request changes from active to deactive */
  #define DEBUG_REQUEST_ON_DEACTIVE(name, code) { static const bool& _debug_request_is_active_ = getDebugRequest().getValueReference(name); static bool _old_debug_request_is_active_ = false; if(_debug_request_is_active_){_old_debug_request_is_active_=true;}else if (_old_debug_request_is_active_) {_old_debug_request_is_active_=false; code}} ((void)0)
  /** Will be executed like a debug request in DEBUG or always in RELEASE */
  #define DEBUG_REQUEST_OR_RELEASE(name, code) DEBUG_REQUEST(name, code)
  /** needed by plot, don't use! */
  #define DEBUG_REQUEST_SLOPPY(name, code) { static const bool& _debug_request_is_active_ = getDebugRequest().registerRequest(name, "", false); if(_debug_request_is_active_){code}} ((void)0)
#else
  /* ((void)0) - that's a do-nothing statement */
  #define DEBUG_REQUEST_REGISTER(name, description, isActiveByDefault) ((void)0)
  #define DEBUG_REQUEST(name, code) ((void)0)
  #define DEBUG_REQUEST_GENERIC(name, code) ((void)0)
  #define DEBUG_REQUEST_ON_DEACTIVE(name, code) ((void)0)
  #define DEBUG_REQUEST_OR_RELEASE(name, code) {code} ((void)0)
#endif // DEBUG

#endif  /* _DEBUGREQUEST_H */

