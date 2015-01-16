/**
* @file SimpleHogFeature.h
*
* @author <a href="mailto:critter@informatik.hu-berlin.de">Claas-Norman Ritter</a>
* Definition of struct SimpleHogFeature
*/

#ifndef _SimpleHogFeature_H_
#define _SimpleHogFeature_H_

#include <opencv2/core/core.hpp>

#include "Representations/Perception/Histograms.h"

#include "Tools/ImageProcessing/Filter.h"

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugImageDrawings.h"

#define IMAGE_GET(image,x,y,p) \
  if(!image.isInside(x,y)) { \
    std::cout << __FILE__ << ":" << __LINE__ << "SCHEISSE!!!" << std::endl; \
  } \
  image.get(x, y, p);

class SimpleHogFeature
{
public:
  
  Statistics::Histogram<9> hog;
  //Statistics::Histogram<9> hoi;
  Pixel pixel;  
  
  SimpleHogFeature(const naoth::Image& image, const Vector2i& point)
  {
    //DEBUG_REQUEST_REGISTER("Vision:Detectors:SimpleHogFeature:mark_HOG", "..", false);
    if
    (
      point.x > 3 && point.y > 3 
      && point.x < (int) image.width() - 4 && point.y < (int) image.height() - 4
    )
    {
      cv::Mat diffX = cv::Mat::zeros(6, 6, CV_64FC1);
      cv::Mat diffY = cv::Mat::zeros(6, 6, CV_64FC1);
     
      int startX = point.x - 3;
      int startY = point.y - 3;

      for(int y = 0; y < 7; y++)
      {
        Filter<Prewitt3x1, Vector2i, double, 3> filterX;
        for(int x = 0; x < 7; x++)
        {
          IMAGE_GET(image, startX + x, startY + y, pixel);
          int diffVU = (int) Math::round(((double) pixel.v - (double)pixel.u) * ((double) pixel.y / 255.0));

          Vector2i posX(x, y);
          filterX.add(posX, diffVU);
          if(x > 1 && y > 1 && x < 7 && y < 7) 
          {
            diffX.at<double>(x - 2, y - 2) = filterX.value(); 
          }
        }
      }

      for(int x = 0; x < 7; x++)
      {
        Filter<Prewitt3x1, Vector2i, double, 3> filterY;
        for(int y = 0; y < 7; y++)
        {
          IMAGE_GET(image, startX + x, startY + y, pixel);
          int diffVU = (int) Math::round(((double) pixel.v - (double)pixel.u) * ((double) pixel.y / 255.0));

          Vector2i posY(x, y);
          filterY.add(posY, diffVU);
          if(x > 1 && y > 1 && x < 7 && y < 7) 
          {
           diffY.at<double>(x - 2, y - 2) = filterY.value(); 
          }
        }
      }

      hog.clear();
      //hoi.clear();

      for(int y = 0; y < 6; y++)
      {
        for(int x = 0; x < 6; x++)
        {
          double valX = diffX.at<double>(x, y);
          double valY = diffY.at<double>(x, y);

          double intensity = sqrt( valX * valX + valY * valY );
          double direction = atan2( valY , valX );

          int g = ((int) ((direction+Math::pi)*9.0/Math::pi2 + 0.5)) % 9;
          //int I = (int) (intensity / 32.0);

          if(g > 9 /*|| I > 9*/)
          {
            //for debug, if g or I are >= 9
            g = g;
          }
          else
          {
            hog.rawData[g] += static_cast<int>(intensity * 1000);
            //hog.add(g);
            //hoi.add(I);
          }
        }
      } 
      hog.calculate();
    }
  }

};

#endif //_SimpleHogFeature_H_
