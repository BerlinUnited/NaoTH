#include "FieldSidePercept.h"

FieldSidePercept::FieldSidePercept()
  : facedFieldSide(unknown)
{
}

void FieldSidePercept::print(std::ostream &stream) const
{
  stream << getFieldSideName(facedFieldSide);
}

FieldSidePercept::~FieldSidePercept()
{

}
