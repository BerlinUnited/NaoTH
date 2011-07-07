/* 
 * File:   FieldPercept.h
 * Author: claas
 *
 * Created on 24. September 2009, 14:05
 */

#ifndef _FIELDPERCEPT_H
#define	_FIELDPERCEPT_H

#include <vector>

#include "Tools/Math/Polygon.h"
#include "Tools/Math/Line.h"
#include <Tools/DataStructures/Printable.h>
#include <Tools/DataStructures/Serializer.h>

#include "Representations/Infrastructure/FrameInfo.h"


#define MAX_FIELD_COUNT 4

class FieldPercept : public naoth::Printable
{
public:
  typedef Math::Polygon<4> FieldRect;
  typedef Math::Polygon<20> FieldPoly;

private:
  FieldPoly fieldPoly;
  FieldRect fieldRect;

  bool valid;
  Vector2<int> dimension;

  // const stuff
  FieldRect fullFieldRect;
  FieldPoly fullFieldPoly;

  void generateDefaultField()
  {
    // create default field rect in image (the whole image) 
    fullFieldRect.add(Vector2<int>(0, 0));
    fullFieldRect.add(Vector2<int>(0, dimension.y - 1));
    fullFieldRect.add(Vector2<int>(dimension.x - 1, dimension.y - 1));
    fullFieldRect.add(Vector2<int>(dimension.x - 1, 0));

    // create default field poly in image (the whole image)
    // basicaly the same as rect
    fullFieldPoly.add(Vector2<int>(0, 0));
    fullFieldPoly.add(Vector2<int>(0, dimension.y - 1));
    fullFieldPoly.add(Vector2<int>(dimension.x - 1, dimension.y - 1));
    fullFieldPoly.add(Vector2<int>(dimension.x - 1, 0));
  }//end generateDefaultField
  
public:

  FieldPercept()
  {
    //be sure the first initiated field is {0,0,0,0} and not {0,0,-1,-1}
    dimension.x = 1;
    dimension.y = 1;
    generateDefaultField();
    reset();
  }

  ~FieldPercept(){}

  const FieldRect& getFullFieldRect() const
  {
    return fullFieldRect;
  }

  const FieldPoly& getFullFieldPoly() const
  {
    return fullFieldPoly;
  }

  void setDimension(Vector2<int> maxDim)
  {
    dimension = maxDim;
    // regenerate the fields
    generateDefaultField();
  }

  bool isValid() const
  {
    return valid;
  }

  void setValid(bool flag)
  {
    valid = flag;
  }

  const FieldPoly& getLargestValidPoly(const Math::LineSegment& horizon) const
  {
    if(isValid())
    {
      return getPoly();
    }
    return getFullFieldPoly();
  }//end getLargestValidPoly

  const FieldRect& getLargestValidRect(const Math::LineSegment& horizon) const
  {
    if(isValid())
    {
      return getRect();
    }
    return getFullFieldRect();
  }//end getLargestValidRect

  bool isRectUnderHorizon(const Math::LineSegment& horizon) const
  {
    cout << endl << "llR " << fieldRect.getArea() << endl;
    for(int i = 0; i < fieldRect.length; i++)
    {
      if(horizon.begin().y < fieldRect[i].y && horizon.end().y < fieldRect[i].y )
      {
        return false;
      }
    }
    return true;
  }//end isPolyUnderHorizont

  bool isPolyUnderHorizon(const Math::LineSegment& horizon) const
  {
    for(int i = 0; i < fieldPoly.length; i++)
    {
      if(horizon.begin().y < fieldPoly[i].y || horizon.end().y < fieldPoly[i].y )
      {
        return false;
      }
    }
    return true;
  }//end isRectUnderHorizont

  const FieldPoly& getPoly() const
  {
    return fieldPoly;
  }//end getLargestPoly

  const FieldRect& getRect() const
  {
    return fieldRect;
  }//end getLargestRect

  void setPoly(const FieldPoly& newField)
	{
    fieldPoly = newField;
	}//end add

  void setRect(const FieldRect& newFieldRect)
	{
    fieldRect = newFieldRect;
	}//end add

  /* reset percept */
  void reset()
  {
    valid = false;
  }//end reset

  virtual void print(ostream& stream) const
  {
    stream << " valid: " << (valid ? "true" : "false") << endl;
    stream << " Rect Area: " << fieldRect.getArea() << endl;
    stream << " Poly Area: " << fieldPoly.getArea() << endl;
    stream << " Poly Vertex Count: " << fieldPoly.length << endl;
  }//end print

};

namespace naoth
{
  template<>
  class Serializer<FieldPercept>
  {
  public:
    static void serialize(const FieldPercept& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, FieldPercept& representation);
  };
}

#endif	/* _FIELDPERCEPT_H */

