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
#include <string>

// TODO: maybe it can be made prinable in the future
#include <ostream>
//#include <Tools/DataStructures/Printable.h>

#include <Tools/Debug/NaoTHAssert.h>
#include <Tools/Math/Common.h>

#include <Representations/Infrastructure/Configuration.h>

//NOTE: it would be goo for ParameterList to be naoth::Printable but it might lead to inheritance issues
class ParameterList 
{
public:
  class ConfigParameter {
  public:
    const std::string name;
    ConfigParameter(const std::string& name) : name(name) {}
  public:
    virtual ~ConfigParameter() {}
    virtual void syncWithConfig(naoth::Configuration& config, const std::string& group) = 0;
    virtual void writeToConfig(naoth::Configuration& config, const std::string& group) const = 0;
    virtual void print(std::ostream& stream) const = 0;
  };

protected:

  template<class T>
  class Parameter : public ConfigParameter 
  {
  public:
    Parameter(const std::string& name) : ConfigParameter(name) {}
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

    virtual void print(std::ostream& stream) const {
      stream << name << "=" << get() << std::endl;
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

  template<class T, class P>
  class CallbackMemberParameter : public DefaultParameter<T> {
  protected:
      P* parent;
      void (P::*callback)(T v);
  public:
    CallbackMemberParameter(const std::string& name, T* value, void (P::*callback)(T v),  P* parent)
      : DefaultParameter<T>(name, value), 
        parent(parent),
        callback(callback)
    {}
    virtual void set(T v) { (parent->*callback)(v); DefaultParameter<T>::set(v); }
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

  template<typename N, class P>
  Parameter<N>& registerParameter(const std::string& parameterName, N& parameter, void (P::*callback)(N))
  {
    CallbackMemberParameter<N,P>* parameterWrapper = new CallbackMemberParameter<N,P>(parameterName, &parameter, callback, reinterpret_cast<P*> (this) );
    parameters.push_back(parameterWrapper);
    return *parameterWrapper;
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

  void print(std::ostream& stream) const 
  {
    for (std::list<ConfigParameter*>::const_iterator iter = parameters.begin(); iter != parameters.end(); ++iter) {
      (**iter).print(stream);
    }
  }

private:
  std::string name;
  std::list<ConfigParameter*> parameters;

  // TODO: du it on per parameter basis
  mutable bool possibly_changed;
};


#define PARAMETER_REGISTER(parameter, ...) registerParameter(convertName(#parameter), parameter, ##__VA_ARGS__)
#define PARAMETER_ANGLE_REGISTER(parameter) registerParameterT<ParameterAngleDegrees>(convertName(#parameter), parameter)

/*
// NOTE: this is a example for the usage of the parameter list
namespace ParameterListTest
{
class MyExampleParameters: public ParameterList
{
public: 
  MyExampleParameters(): ParameterList("MyExampleParameters")
  {
    PARAMETER_REGISTER(boolParameter) = false;
    PARAMETER_REGISTER(intParameter) = 42;
    PARAMETER_REGISTER(doubleParameter) = 3.14;
    PARAMETER_REGISTER(stringParameter) = "test"; // ms
    PARAMETER_REGISTER(intParameter, &MyExampleParameters::setIntParameter) = 42;

    PARAMETER_REGISTER(intParameter) = 1000;
    PARAMETER_REGISTER(intParameterWithCallback, &MyExampleParameters::setIntParameter) = 1000;

    // load from the file after registering all parameters
    syncWithConfig();
  }
  virtual ~MyExampleParameters() {}

  bool boolParameter;
  int intParameter;
  double doubleParameter;
  std::string stringParameter;

  int intParameterWithCallback;
  inline void setIntParameter(int v) { std::cout << "old: " << intParameterWithCallback << " new: " << v << std::endl; }

} myExampleParameters;
};

//*/

#endif // _ParameterList_h_
