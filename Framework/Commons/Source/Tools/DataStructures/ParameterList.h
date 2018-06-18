/**
 * @file ParameterList.h
 *
 * @author <a href="mailto:aschulz@informatik.hu-berlin.de">Alexander Schulz</a>
 * @author <a href="mailto:krause@informatik.hu-berlin.de">Thomas Krause</a>
 * This class is intendes as superclass of all Representations holding parameters that can be changed
 * via the debug-console
 */

#ifndef _ParameterList_h_
#define _ParameterList_h_

#include <map>
#include <list>
#include <sstream>
#include <Tools/Debug/NaoTHAssert.h>
#include <Tools/Math/Common.h>
#include <Representations/Infrastructure/Configuration.h>

class ParameterList
{
protected:

  class ConfigParameter {
  public:
    virtual ~ConfigParameter() {}
    virtual void syncWithConfig(naoth::Configuration& config, const std::string& group) = 0;
    virtual void writeToConfig(naoth::Configuration& config, const std::string& group) const = 0;
  };

  template<class T>
  class Parameter : public ConfigParameter {
  private:
    const std::string name;
  public:
    Parameter(const std::string& name) : name(name) {}
    virtual ~Parameter() {}

    virtual void syncWithConfig(naoth::Configuration& config, const std::string& group) {
      T v;
      if (config.get(group, name, v)) {
        set(v);
      } else {
        config.setDefault(group, name, get());
      }
    }
    virtual void writeToConfig(naoth::Configuration& config, const std::string& group) const {
      config.set(group, name, get());
    }

    void operator=( const T& v ) { set(v); }

    virtual void set(T v) = 0;
    virtual T get() const = 0;
  };

  template<class T>
  class DefaultParameter : public Parameter<T> {
  protected:
    T* value;

  public:
    DefaultParameter(const std::string& name, T* value) : Parameter<T>(name), value(value) {}
    ~DefaultParameter(){}
    virtual void set(T v) { *value = v; }
    virtual T get() const { ASSERT(value != NULL); return *value; }
  };

  template<class T>
  class ParameterAngleDegrees : public Parameter<T> {
    protected:
      T* value;
    public:
      ParameterAngleDegrees(const std::string& name, T* value) : Parameter<T>(name), value(value) {}
      ~ParameterAngleDegrees(){}
      virtual void set(T v) { *value = Math::fromDegrees(v); }
      virtual T get() const { ASSERT(value != NULL); return Math::toDegrees(*value); }
  };

protected:
  // ACHTUNG: never copy the content of the parameter list
  ParameterList(const ParameterList& /*obj*/) {}
  ParameterList& operator=( const ParameterList& /*other*/ ) { return *this; }

  ParameterList(const std::string& name) : name(name), possibly_changed(true) {}
  virtual ~ParameterList() 
  {
    for (std::list<ConfigParameter*>::iterator iter = parameters.begin(); iter != parameters.end(); ++iter) {
      delete *iter;
    }
  }

  template<template<typename N> class T, typename N>
  Parameter<N>& registerParameterT(const std::string& parameterName, N& parameter)
  {
    T<N>* parameterWrapper = new T<N>(parameterName, &parameter);
    parameters.push_back(parameterWrapper);
    return *parameterWrapper;
  }

  template<typename N>
  Parameter<N>& registerParameter(const std::string& parameterName, N& parameter) {
    return registerParameterT<DefaultParameter,N>(parameterName, parameter);
  }

  // change some key characters, e.g. []
  static std::string convertName(std::string name);

public:
  void syncWithConfig();
  void saveToConfig();

  const std::string& getName() const { return name; }

  // check if any parametsr were changed and reset the flag
  bool check_changed() const { 
    bool change = possibly_changed; 
    possibly_changed = false;
    return change; 
  }

private:
  std::string name;
  std::list<ConfigParameter*> parameters;

  // TODO: du it on per parameter basis
  mutable bool possibly_changed;
};


#define PARAMETER_REGISTER(parameter) registerParameterT<DefaultParameter>(convertName(#parameter), parameter)
#define PARAMETER_ANGLE_REGISTER(parameter) registerParameterT<ParameterAngleDegrees>(convertName(#parameter), parameter)

#endif // _ParameterList_h_
