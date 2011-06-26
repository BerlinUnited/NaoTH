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

  typedef Math::Polygon<4> FieldRect;
  typedef Math::Polygon<20> FieldPoly;

private:
  struct Field
  {
    FieldPoly polygon;
    FieldRect rectangle;
    double area;
  };

  vector<Field> fields;
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
    fields.reserve(MAX_FIELD_COUNT);
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

  unsigned int count() const
  {
    return fields.size();
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
      return getLargestPoly(horizon);
    }
    return getFullFieldPoly();
  }//end getLargestValidPoly

  const FieldRect& getLargestValidRect(const Math::LineSegment& horizon) const
  {
    if(isValid())
    {
      return getLargestRect(horizon);
    }
    return getFullFieldRect();
  }//end getLargestValidRect

  bool isPolyUnderHorizon(unsigned int idx, const Math::LineSegment& horizon) const
  {
    for(int i = 0; i < fields[idx].rectangle.length; i++)
    {
      if(horizon.begin().y < fields[idx].rectangle[i].y || horizon.end().y < fields[idx].rectangle[i].y )
      {
        return false;
      }
    }
    return true;
  }//end isPolyUnderHorizont

  bool isRectUnderHorizon(unsigned int idx, const Math::LineSegment& horizon) const
  {
    for(int i = 0; i < fields[idx].polygon.length; i++)
    {
      if(horizon.begin().y < fields[idx].polygon[i].y || horizon.end().y < fields[idx].polygon[i].y )
      {
        return false;
      }
    }
    return true;
  }//end isRectUnderHorizont

  const FieldPoly& getLargestPoly(const Math::LineSegment& horizon) const
  {
    if(count() > 0)
    {
      double maxArea = 0.0;
      unsigned int index = 0;
      for(unsigned int i = 0; i < count(); i++)
      {
        if(maxArea <= fields[i].area && isPolyUnderHorizon(i, horizon))
        {
          maxArea = fields[i].area;
          index = i;
        }
      }
      return fields[index].polygon;
    }
    return getFullFieldPoly();
  }//end getLargestPoly

  const FieldRect& getLargestRect(const Math::LineSegment& horizon) const
  {
    if(count() > 0)
    {
      double maxArea = 0.0;
      unsigned int index = 0;
      for(unsigned int i = 0; i < count(); i++)
      {
        if(maxArea <= fields[i].area && isRectUnderHorizon(i, horizon))
        {
          maxArea = fields[i].area;
          index = i;
        }
      }
      return fields[index].rectangle;
    }
    return getFullFieldRect();
  }//end getLargestRect

  const FieldPoly& getPoly(unsigned int index) const
  {
    if(index < count())
    {
      return fields[index].polygon;
    }
    return getFullFieldPoly();
  }//end getPoly

  const FieldRect& getRect(unsigned int index) const
  {
    if(index < count())
    {
      return fields[index].rectangle;
    }
    return getFullFieldRect();
  }//end getRect


  bool add(const FieldPoly& newField, const FieldRect& newFieldRect)
	{
    if( fields.size() < MAX_FIELD_COUNT )
    {
      Field field;
      field.rectangle = newFieldRect;
      field.polygon = newField;
      field.area = newField.getArea();
      fields.push_back(field);
      return true;
    }
    return false;
	}//end add

  /* reset percept */
  void reset()
  {
    fields.clear();
    valid = false;
  }//end reset

  virtual void print(ostream& stream) const
  {
    stream << "number of seen fields = " << count() << "/" << MAX_FIELD_COUNT << endl;
    stream << " valid: " << (valid ? "true" : "false") << endl;
    for(unsigned int i = 0; i < count(); i++)
    {
      int len = fields[i].polygon.length;

      stream << "Field " << (i + 1) << ":" << endl;
      stream << " Area: " << fields[i].area << endl;
      stream << " Vertex Count: " << len << endl;
    }
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

