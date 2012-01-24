#ifndef FIELDSIDEPERCEPT_H
#define FIELDSIDEPERCEPT_H

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
  virtual void print(ostream &stream) const;

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

#endif // FIELDSIDEPERCEPT_H
