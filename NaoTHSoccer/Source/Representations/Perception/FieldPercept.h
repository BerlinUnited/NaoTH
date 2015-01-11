/* 
 * File:   FieldPercept.h
 * Author: claas
 *
 * Created on 24. September 2009, 14:05
 */

#ifndef _FIELDPERCEPT_H
#define _FIELDPERCEPT_H

#include <vector>

#include "Tools/Math/Polygon.h"
#include "Tools/Math/Line.h"
#include <Tools/DataStructures/Printable.h>
#include <Tools/DataStructures/Serializer.h>


class FieldPercept : public naoth::Printable
{
public:
  typedef Math::Polygon<20> FieldPoly;

private:
  FieldPoly fieldPoly;
  Vector2i dimension;

  // maximal field polygon containing the full image
  FieldPoly fullFieldPoly;

  void generateDefaultField()
  {
    fullFieldPoly.clear();
    // create default field poly in image (the whole image)
    // basically the same as rect
    fullFieldPoly.add(Vector2i(0, 0));
    fullFieldPoly.add(Vector2i(0, dimension.y - 1));
    fullFieldPoly.add(Vector2i(dimension.x - 1, dimension.y - 1));
    fullFieldPoly.add(Vector2i(dimension.x - 1, 0));
    fullFieldPoly.add(Vector2i(0, 0));
  }//end generateDefaultField

public:
  bool valid;

  FieldPercept()
  :
    dimension(2,2),
    valid(false)
  {
    //be sure the first initiated field is {0,0,0,0} and not {0,0,-1,-1}
    generateDefaultField();
    reset();
  }

  virtual ~FieldPercept(){}

  const FieldPoly& getFullField() const {
    return fullFieldPoly;
  }

  void setDimension(const Vector2i& maxDim)
  {
    dimension = maxDim;
    // regenerate the fields
    generateDefaultField();
  }

  const FieldPoly& getField() const {
    return fieldPoly;
  }

  const FieldPoly& getValidField() const
  {
    if(valid) {
      return getField();
    }
    return getFullField();
  }

  void checkPolyIsUnderHorizon(const Math::LineSegment& horizon)
  {
    for(int i = 0; i < fieldPoly.length; i++)
    {
      Vector2i projectedPoint(horizon.projection(fieldPoly[i])); 

      if(projectedPoint.y > fieldPoly[i].y) {
        fieldPoly[i].y = projectedPoint.y;  
      }
    }
  }

  void setField(const FieldPoly& newField, const Math::LineSegment& horizon)
  {
    fieldPoly = newField;
    checkPolyIsUnderHorizon(horizon);
  }

  void reset()
  {
    valid = false;
  }

  virtual void print(std::ostream& stream) const
  {
    stream << " valid: " << (valid ? "true" : "false") << std::endl;
    stream << " Field Area: " << fieldPoly.getArea() << std::endl;
    stream << " Poly Vertex Count: " << fieldPoly.length << std::endl;
  }
};


class FieldPerceptRaw : public FieldPercept{};
class FieldPerceptRawTop : public FieldPerceptRaw{};

class FieldPerceptTop : public FieldPercept{};



#endif /* _FIELDPERCEPT_H */

