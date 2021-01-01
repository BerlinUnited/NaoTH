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
  typedef Math::Polygon<int> FieldPoly;

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
    fullFieldPoly.add(0, 0);
    fullFieldPoly.add(0, dimension.y - 1);
    fullFieldPoly.add(dimension.x - 1, dimension.y - 1);
    fullFieldPoly.add(dimension.x - 1, 0);
    fullFieldPoly.add(0, 0);
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

  int getMinY() const 
  {
    const FieldPoly& fieldPolygon = getValidField();

    // find the top point of the polygon
    int minY = std::numeric_limits<int>::max();
    for(size_t i = 0; i < fieldPolygon.size(); i++)
    {
      if(fieldPolygon[i].y < minY && fieldPolygon[i].y >= 0) {
        minY = fieldPolygon[i].y;
      }
    }

    return minY;
  }

  // TODO: move this method to the field detectors or to a tool class
  void checkPolyIsUnderHorizon(const Math::LineSegment& horizon)
  {
    for(size_t i = 0; i < fieldPoly.size(); ++i)
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

  void setField(const FieldPoly& newField)
  {
    fieldPoly = newField;
  }

  void reset()
  {
    valid = false;
  }

  virtual void print(std::ostream& stream) const
  {
    stream << " valid: " << (valid ? "true" : "false") << std::endl;
    stream << " Field Area: " << fieldPoly.getArea() << std::endl;
    stream << " Poly Vertex Count: " << fieldPoly.size() << std::endl;
  }
};

class FieldPerceptRaw : public FieldPercept{};
class FieldPerceptRawTop : public FieldPerceptRaw{};

class FieldPerceptTop : public FieldPercept{};

namespace naoth
{
template<>
class Serializer<FieldPercept>
{
  public:
  static void serialize(const FieldPercept& object, std::ostream& stream);
  static void deserialize(std::istream& stream, FieldPercept& object);
};

template<>
class Serializer<FieldPerceptTop> : public Serializer<FieldPercept>
{};
}

#endif /* _FIELDPERCEPT_H */

