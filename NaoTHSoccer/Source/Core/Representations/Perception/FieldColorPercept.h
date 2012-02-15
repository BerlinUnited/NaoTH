/* 
 * File:   FieldColorPercept.h
 * Author: claas
 *
 * Created on 17. März 2011, 22:53
 */

#ifndef FIELDCOLORPERCEPT_H
#define  FIELDCOLORPERCEPT_H

#include <Tools/DataStructures/Printable.h>
#include <Representations/Infrastructure/FrameInfo.h>
//#include <Tools/DataStructures/Serializer.h>

#define MAX_FIELD_COLOR_Y_CHANNEL_DIST 48
#define MAX_FIELD_COLOR_Cb_CHANNEL_DIST 12
#define MAX_FIELD_COLOR_Cr_CHANNEL_DIST 12
#define MIN_FIELD_COLOR_Y_CHANNEL_DIST 20
#define MIN_FIELD_COLOR_Cb_CHANNEL_DIST 2
#define MIN_FIELD_COLOR_Cr_CHANNEL_DIST 2
#define MIN_FIELD_COLOR_Y_LEVEL 20
#define MAX_FIELD_COLOR_Y_LEVEL 224

using namespace naoth;

class FieldColorPercept : public naoth::Printable
{
  
public:
  double distY;
  double distCb;
  double distCr;
  double maxWeightedY;
  double maxWeightedCb;
  double maxWeightedCr;
  unsigned int maxWeightedIndexY;
  unsigned int maxWeightedIndexCb;
  unsigned int maxWeightedIndexCr;

  double minWhite;
  double maxBlack;

  FrameInfo lastUpdated;

  FieldColorPercept()
  {
    distY = MAX_FIELD_COLOR_Y_CHANNEL_DIST;
    distCb = MAX_FIELD_COLOR_Cb_CHANNEL_DIST;
    distCr = MAX_FIELD_COLOR_Cr_CHANNEL_DIST;
    maxWeightedY = 0.0;
    maxWeightedCb = 0.0;
    maxWeightedCr = 0.0;
    maxWeightedIndexY = 160;
    maxWeightedIndexCb = 160;
    maxWeightedIndexCr = 160;

    minWhite = 0.0;
    maxBlack = 0.0;

  }

  ~FieldColorPercept()
  {}

  bool isFieldColor(const unsigned int& yy, const unsigned int& cb, const unsigned int& cr) const
  {
    return
      (
        abs((int)(cr  - distCr) < (int) maxWeightedIndexCr)
        &&
        abs((int)(cb  - distCb) < (int) maxWeightedIndexCb)
        &&
        abs((int)(yy  - distY) < (int) maxWeightedIndexY)
//        &&
//        yy < minWhite
      );
  }

  bool isFieldColor(const Pixel& pixel) const
  {
    return isFieldColor(pixel.y, pixel.u, pixel.v);
  }


  bool isOnlyFieldColor(const unsigned int& yy, const unsigned int& cb, const unsigned int& cr, const ColorClasses::Color& color) const
  {
    return
      (
        isFieldColor(yy, cb, cr) &&
        color != ColorClasses::white &&
        color != ColorClasses::orange &&
        color != ColorClasses::skyblue &&
        color != ColorClasses::yellow
      );
  }

  bool isOnlyFieldColor(const Pixel& pixel, const ColorClasses::Color& color) const
  {
    return isOnlyFieldColor(pixel.y, pixel.u, pixel.v, color);
  }

  virtual void print(ostream& stream) const
  {
    stream << "distance in Y channel = " << distY << endl;
    stream << "distance in Cb channel = " << distCb << endl;
    stream << "distance in Cr channel = " << distCr << endl;
    stream << "max rate value in Y channel = " << maxWeightedY << endl;
    stream << "max rate value in Cb channel = " << maxWeightedCb << endl;
    stream << "max rate value in Cr channel = " << maxWeightedCr << endl;
    stream << "max value in Y channel = " << maxWeightedIndexY << endl;
    stream << "max value in Cb channel = " << maxWeightedIndexCb << endl;
    stream << "max value in Cr channel = " << maxWeightedIndexCr << endl;
  }//end print

};


#endif  /* FIELDCOLORPERCEPT_H */

