/**
* @file Tools/ImageProcessing/SpiderScan.h
* 
* Utility class which performs a SpiderScan algorithm for blob region scanning
*
*
*/

#ifndef __SpiderScan_h_
#define __SpiderScan_h_

#include <vector>

//#include "Cognition/Cognition.h"

#include "Tools/ColorClasses.h"
#include "Tools/Math/Vector2.h"
//#include "Tools/Math/Matrix_nxn.h"
#include "Tools/Math/PointList.h"

#include "Representations/Infrastructure/Image.h"
#include "Representations/Infrastructure/ColorTable64.h"

class SpiderScan
{
public:
	SpiderScan(const Image& theImage, const ColorTable64& theColorTable64);
	SpiderScan(const Image& theImage, const ColorTable64& theColorTable64, ColorClasses::Color searchColor);
	SpiderScan(const Image& theImage, const ColorTable64& theColorTable64, vector<ColorClasses::Color>& searchColors);
	SpiderScan(const Image& theImage, const ColorTable64& theColorTable64, ColorClasses::Color searchColor, ColorClasses::Color borderColor);
	SpiderScan(const Image& theImage, const ColorTable64& theColorTable64, vector<ColorClasses::Color>& searchColors, vector<ColorClasses::Color>& borderColors);
  void init();

  class Scans
  {
  public:
    Scans() : number(0) {};
    enum{ MAXNUMBEROFSCANS=40};
    Vector2<int> start[MAXNUMBEROFSCANS];
    Vector2<int> direction[MAXNUMBEROFSCANS];
    unsigned int number;

    void add(const Vector2<int>& point, const Vector2<int>& dir)
    {
      if(number < MAXNUMBEROFSCANS)
      {
        start[number] = point;
        direction[number] = dir;
        number++;
      }
    }//end add

    void setStart(const Vector2<int>& startPoint)
    {
      for(unsigned int i  = 0; i < number; i++)
      {
        start[i] = startPoint;
      }
    }//end add

    void remove(unsigned int index)
    {
      if(index < number)
      {
        start[index] = start[number - 1];
        direction[index] = direction[number - 1];
        number--;
      }
    }//end remove
  };//end class Scans

  void scan(const Vector2<int>& start, PointList<20>& goodPoints, PointList<20>& badPoints);
  void scan(PointList<20>& goodPoints, PointList<20>& badPoints, Scans scans);
  void setDrawScanLines(bool draw);
  void setMaxBeamLength(unsigned int length);
  void setCurrentColorSimThreshold(double threshold);
  void setMaxColorPointsToSkip(unsigned int length);
  void setMaxNumberOfScans(unsigned int length);

private:
  bool scanLine(const Vector2<int>& start, const Vector2<int>& direction, int maxColorPointsToSkip, PointList<20>& goodPoints, PointList<20>& badPoints) const;
  inline bool isBorderScan(const Vector2<int>& point, const Vector2<int>& direction, int borderWidth) const;

  inline bool isBorderColor(ColorClasses::Color color) const;
  inline bool isSearchColor(ColorClasses::Color color) const;

  inline bool pixelInImage(const Vector2<int>& pixel) const;
  inline bool pixelAtImageBorder(const Vector2<int>& pixel, int borderWidth) const;

  const Image& theImage;
  const ColorTable64& theColorTable64;

  bool drawScanLines;
  vector<ColorClasses::Color> searchColors;
  vector<ColorClasses::Color> borderColors;
  unsigned int max_length_of_beam;
  double currentColorSimThreshold;
  int maxColorPointsToSkip; // maximum number of non search color ...
  int maxNumberOfScans; //maximum number of scanlines ...


};


#endif //__SpiderScan_h_
