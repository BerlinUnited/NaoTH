#ifndef _FieldSidePercept_H_
#define _FieldSidePercept_H_

#include <Tools/DataStructures/Printable.h>

using namespace naoth;

class FieldSidePercept : public Printable
{
public:

  enum FieldSide
  {
    own,
    opponent,
    unknown
  };

  FieldSidePercept();
  virtual ~FieldSidePercept();
  virtual void print(std::ostream &stream) const;

  static const char* getFieldSideName(FieldSide side)
  {
    switch(side)
    {
      case own:   return "own";
      case opponent:    return "opponent";
      case unknown: return "unknown";
      default: return "unknown field side type!";
    };
  }//end getPostTypeName

public:
  FieldSide facedFieldSide;

};

#endif // _FieldSidePercept_H_
