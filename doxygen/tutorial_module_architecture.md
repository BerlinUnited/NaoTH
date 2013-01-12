Module Framework {#tutorials_module_architecture}
=====================

The module framework is based on the principle of a blackboard architercure.
It consists of following main compinents:
- Module: 
  is a basic execution unit (execute algorithms);
- Representation:
  hold data which can be exchanged between modules;
- BlackBoard:
  it holds the representations and provides access for modules (can be seen as a central data base);
- ModuleManager:
  handles the execution flow and holds the BlackBoard;

Here is a schematic depiction of the dependency of those components. 
  
![Blackboard Architercure](blackboard.svg)


General Considerations and Design Principles
---------------------
- information flow
- execution flow

- Modules should not depend on each other directly
- Representation should have as less functionality as possible



Example Project
---------------------
~~~~~~~~~~~~~~~~~~~~~
Core
|-- Modules
|   `-- ModuleA.h
|-- Representations
|   |-- RepresentationA.h
|   `-- RepresentationB.h
`-- Cognition.h
~~~~~~~~~~~~~~~~~~~~~


Creating a simple Representation
---------------------
Every class can be used as a representation as long as it fulfills following requirements:
- it's not abstract 
- it has a public default constructor

~~~~~~~~~~~~~~~~~~~~~{.cpp}

//
class RepresentationA
{
public:
  RepresentationA()
    : x(0)
  {
  }
  
  int x;
};

~~~~~~~~~~~~~~~~~~~~~


Creating a Simple Module
---------------------
Following example shows the definition of a simple module `ModuleA`, 
which requires the representation `RepresentationA` and 
provides the representation `RepresentationB`.

~~~~~~~~~~~~~~~~~~~~~{.cpp}

//
#include <ModuleFramework/Module.h>

// include some representations
#include "RepresentationA.h"
#include "RepresentationB.h"

// this creates the base class ModuleABase
BEGIN_DECLARE_MODULE(ModuleA)
  REQUIRE(RepresentationA)
  PROVIDE(RepresentationB)
END_DECLARE_MODULE(ModuleA)


//
class ModuleA: private ModuleABase
{

public:

  // a default constructor is required,
  // it is used to create an instance of the module
  ModuleA()
  {
  }

  // the execute method is called to run the module
  // put your functionality here
  virtual void execute()
  {
    // do something with RepresentationA and RepresentationB:
    // int x = getRepresentationA().x;
    // getRepresentationB().y = x + 1;
  }

};//end class ModuleA

~~~~~~~~~~~~~~~~~~~~~


Using the ModuleManager
---------------------

~~~~~~~~~~~~~~~~~~~~~{.cpp}

//
#include <ModuleFramework/ModuleManager.h>

//
#include "ModuleA.h"

class Cognition : public ModuleManager
{
public:
  Cognition();
  virtual ~Cognition();

  void call();

};

~~~~~~~~~~~~~~~~~~~~~