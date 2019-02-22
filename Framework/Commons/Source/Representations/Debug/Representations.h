/* 
 * File:   Representations.h
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 *
 */

#ifndef _Representations_H_
#define _Representations_H_

#include <map>
#include <cstring>
#include <string>

#include "ModuleFramework/BlackBoard.h"

#include "Tools/DataStructures/Serializer.h"
#include "Tools/DataStructures/Printable.h"

class Representations: virtual public BlackBoardInterface, public naoth::Printable
{
public:
  virtual void print(std::ostream& stream) const
  {
    for(const auto& r: getBlackBoard().getRegistry()) {
      stream << r.first << std::endl;
    }
  }

  void setRepresentation(std::ostream &outstream, const std::string& name, const std::string& data)
  {
    BlackBoard::Registry::const_iterator iter = getBlackBoard().getRegistry().find(name);
    if(iter != getBlackBoard().getRegistry().end()) {
      std::stringstream ss(data);
      iter->second->getRepresentation().deserialize(ss);
    } else {
      outstream << "unknown representation" << std::endl;
    }
  }

  void printRepresentation(std::ostream &outstream, const std::string& name)
  {
    BlackBoard::Registry::const_iterator iter = getBlackBoard().getRegistry().find(name);
    if(iter != getBlackBoard().getRegistry().end()) {
        iter->second->getRepresentation().print(outstream);
    } else {
      outstream << "unknown representation" << std::endl;
    }
  }//end printRepresentation

  void serializeRepresentation(std::ostream &outstream, const std::string& name)
  {
    BlackBoard::Registry::const_iterator iter = getBlackBoard().getRegistry().find(name);
    if(iter != getBlackBoard().getRegistry().end()) {
        iter->second->getRepresentation().serialize(outstream);
    } else {
      outstream << "unknown representation" << std::endl;
    }
  }

  void representationList(std::ostream &outstream)
  {
    BlackBoard::Registry::const_iterator iter = getBlackBoard().getRegistry().begin();
    for(; iter != getBlackBoard().getRegistry().end(); ++iter) {
      outstream << iter->first << std::endl;
    }
  }
};

namespace naoth
{
template<>
class Serializer<Representations>
{
  public:
  static void serialize(const Representations& object, std::ostream& stream);

  // we don't need that
  static void deserialize(std::istream& stream, Representations& object);
};
}

#endif  /* _Representations_H_ */

