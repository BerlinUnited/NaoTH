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

#include "Representations/Infrastructure/Image.h"

#include "Tools/DataStructures/RingBufferWithSum.h"

using namespace std;
using namespace naoth;

class GradientSpiderScan
{
public:
  typedef PointList<20> ScanPointList;

  GradientSpiderScan(const Image& theImage, CameraInfo::CameraID camID);

  void init();

  class Scans
  {
  public:
    Scans() : number(0) {};
    enum{ MAXNUMBEROFSCANS=40};
    Vector2i start[MAXNUMBEROFSCANS];
    Vector2i direction[MAXNUMBEROFSCANS];
    unsigned int number;

    void add(const Vector2i& point, const Vector2i& dir)
    {
      if(number < MAXNUMBEROFSCANS)
      {
        start[number] = point;
        direction[number] = dir;
        number++;
      }
    }//end add

    void setStart(const Vector2i& startPoint)
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

  void scan(const Vector2i& start, ScanPointList& goodPoints, ScanPointList& badPoints);
  void scan(ScanPointList& goodPoints, ScanPointList& badPoints, Scans scans);
  void setDrawScanLines(bool draw);
  void setMaxBeamLength(unsigned int length);
  void setCurrentGradientThreshold(double threshold);
  void setDynamicThresholdY(double threshold);
  void setCurrentMeanThreshold(double threshold);
  void setImageColorChannelNumber(int channelNumber);
  void setMaxNumberOfScans(unsigned int length);
  void setUseVUdifference(bool value);
  void setMinChannelValue(double value);

private:
  Pixel startPixel;
  bool scanLine(const Vector2i& start, const Vector2i& direction, ScanPointList& goodPoints, ScanPointList& badPoints);
  inline bool isBorderScan(const Vector2i& point, const Vector2i& direction, int borderWidth) const;

  inline bool pixelInImage(const Vector2i& pixel) const;
  inline bool pixelAtImageBorder(const Vector2i& pixel, int borderWidth) const;  

  const Image& theImage;
  RingBufferWithSum<int, 24> scanPixelBuffer;

  bool drawScanLines;
  unsigned int max_length_of_beam;
  double currentGradientThreshold;
  double dynamicThresholdY;
  bool useDynamicThresholdY;
  int imageChannelNumber;
  int imageChannelValidate;
  int maxNumberOfScans; //maximum number of scanlines ...
  double maxChannelDif;
  bool useVUdifference;
  CameraInfo::CameraID cameraID;
  double minChannelValue;
};


#endif //__GradientSpiderScan_h_
