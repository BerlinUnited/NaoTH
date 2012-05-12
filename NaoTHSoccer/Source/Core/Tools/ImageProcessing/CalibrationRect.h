#ifndef CALIBRATIONRECT_H
#define CALIBRATIONRECT_H

#include <Tools/Debug/DebugParameterList.h>
#include <Tools/Debug/DebugImageDrawings.h>
#include <Tools/ColorClasses.h>
#include <Tools/Math/Vector2.h>

using namespace naoth;

class CalibrationRect : public ParameterList
{
public:
  Vector2<int> lowerLeft;
  Vector2<int> upperRight;
  ColorClasses::Color color;

  CalibrationRect(std::string rectName, ColorClasses::Color rectColor, int x0, int y0, int x1, int y1)
  : 
    ParameterList("CalibrationAreaRect_" + rectName),
    lowerLeft(x0, y0),
    upperRight(x1, y1),
    color(rectColor)
  {
    PARAMETER_REGISTER(lowerLeft.x) = lowerLeft.x;  
    PARAMETER_REGISTER(lowerLeft.y) = lowerLeft.y;  
    PARAMETER_REGISTER(upperRight.x) = upperRight.x;  
    PARAMETER_REGISTER(upperRight.y) = upperRight.y;        
    syncWithConfig();  

    registerParameters();
  }

  void registerParameters()
  {
     DebugParameterList::getInstance().add(this);
  }

  ~CalibrationRect()
  {
    DebugParameterList::getInstance().remove(this);
  }

  void draw()
  {
    RECT_PX(color, lowerLeft.x, lowerLeft.y, upperRight.x, upperRight.y);
  }

};


#endif // CALIBRATIONRECT_H
