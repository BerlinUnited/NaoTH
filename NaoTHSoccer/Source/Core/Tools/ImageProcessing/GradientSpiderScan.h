/**
* @file Tools/ImageProcessing/GradientSpiderScan.h
* 
* Utility class which performs a GradientSpiderScan algorithm for blob region scanning
*
*
*/

#ifndef __GradientSpiderScan_h_
#define __GradientSpiderScan_h_

#include <vector>

#include "Tools/ColorClasses.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Math/PointList.h"

//#include "ColorClassifier.h"


#include "Representations/Infrastructure/Image.h"
//#include "Representations/Infrastructure/ColorTable64.h"
#include "Representations/Modeling/ColorClassificationModel.h"

#include "Tools/DataStructures/RingBufferWithSum.h"

using namespace std;
using namespace naoth;

class GradientSpiderScan
{
public:
  GradientSpiderScan(const Image& theImage, CameraInfo::CameraID camID);

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
  void setCurrentGradientThreshold(double threshold);
  void setDynamicThresholdY(double threshold);
  void setCurrentMeanThreshold(double threshold);
  void setImageColorChannelNumber(int channelNumber);
  void setMaxNumberOfScans(unsigned int length);

private:
  Pixel startPixel;
  bool scanLine(const Vector2<int>& start, const Vector2<int>& direction, PointList<20>& goodPoints, PointList<20>& badPoints);
  inline bool isBorderScan(const Vector2<int>& point, const Vector2<int>& direction, int borderWidth) const;

  inline bool pixelInImage(const Vector2<int>& pixel) const;
  inline bool pixelAtImageBorder(const Vector2<int>& pixel, int borderWidth) const;  

  const Image& theImage;
  RingBufferWithSum<int, 24> scanPixelBuffer;

  bool drawScanLines;
  unsigned int max_length_of_beam;
  double currentGradientThreshold;
  double currentMeanThreshold;
  double dynamicThresholdY;
  bool useDynamicThresholdY;
  int imageChannelNumber;
  int imageChannelValidate;
  int maxNumberOfScans; //maximum number of scanlines ...
  double maxChannelDif;
  CameraInfo::CameraID cameraID;
};


#endif //__GradientSpiderScan_h_
