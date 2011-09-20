/**
 * @file Cognition.h
 *
  * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#ifndef COGNITION_H
#define  COGNITION_H

#include <iostream>

#include <PlatformInterface/Callable.h>
#include <PlatformInterface/PlatformInterface.h>
#include <ModuleFramework/ModuleManager.h>
#include <DebugCommunication/DebugCommandExecutor.h>

#include "Tools/Packages/PackageLoader.h"
#include <Tools/Debug/Stopwatch.h>

#include "Tools/Debug/Logger.h"

namespace naoth
{
  template<>
  class Serializer<Representation>
  {
  public:
    static void serialize(const Representation& representation, std::ostream& stream)
    {
      representation.serialize(stream);
    }
    static void deserialize(std::istream& stream, Representation& representation)
    {
      representation.deserialize(stream);
    }
  };
}

class Cognition : public naoth::Callable, public ModuleManager, public DebugCommandExecutor
{
public:
  Cognition();
  virtual ~Cognition();

  virtual void call();

  void init(naoth::PlatformInterfaceBase& platformInterface);

  void executeDebugCommand(const std::string& command, 
                           const std::map<std::string,std::string>& arguments, 
                           std::ostream& outstream);

private:
  PackageLoader packageLoader;
  StopwatchItem stopwatch;
  
  void printRepresentation(std::ostream &outstream, const std::string& name, bool binary)
  {
    const BlackBoard& blackBoard = getBlackBoard();
    BlackBoard::Registry::const_iterator iter = blackBoard.getRegistry().find(name);

    if(iter != blackBoard.getRegistry().end())
    {
      const Representation& theRepresentation = iter->second->getRepresentation();
      
      if(binary)
      {
        //theRepresentation.toDataStream(outstream);
      }
      else
      {
        theRepresentation.print(outstream);
      }
    }else
    {
      outstream << "unknown representation" << endl;
    }
  }//end printRepresentation


  void printRepresentationList(std::ostream &outstream)
  {
    const BlackBoard& blackBoard = getBlackBoard();
    BlackBoard::Registry::const_iterator iter;

    for(iter = blackBoard.getRegistry().begin(); iter != blackBoard.getRegistry().end(); ++iter)
    {
      outstream << iter->first << endl;
    }
  }//end printRepresentationList
};

#endif  /* COGNITION_H */

