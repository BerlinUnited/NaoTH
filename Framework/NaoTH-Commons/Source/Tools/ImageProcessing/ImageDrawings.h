/**
* @file ImageDrawings.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class ImageDrawings
*
* TODO: how to use
* ...
*/
#ifndef __ImageDrawings_H_
#define __ImageDrawings_H_

#include <stdlib.h>

namespace naoth
{

class DrawingCanvas
{
public:
  virtual void drawPoint
  (
    const unsigned int& x,
    const unsigned int& y,
    const unsigned char& a,
    const unsigned char& b,
    const unsigned char& c
  ) = 0;

  virtual ~DrawingCanvas(){}

  virtual unsigned int width() = 0;
  virtual unsigned int height() = 0;
};


class ImageDrawings
{

public:
  /*Draws a Circle 
   * @param image Image to draw in
   * @param startX X Coordinate of the Center Point of the Circle  
   * @param startY Y Coordinate of the Center Point of the Circle   
   * @param radius Radius of the Circle to draw
   * @param (a,b,c) Color of the Circle
  */
  void static drawCircleToImage
  (
    DrawingCanvas& image,
    const unsigned int& startX,
    const unsigned int& startY,
    const unsigned int& radius,
    const unsigned char& a,
    const unsigned char& b,
    const unsigned char& c
  )
  {
    int xC = 0; 
    int yC = radius; 
    int d = 1-radius;
    int dx = 3; 
    int dxy = -2*radius+5;

    while ( yC >= xC )
    {
      drawPointToImage(image, startX+xC, startY+yC, a, b, c);
      drawPointToImage(image, startX+yC, startY+xC, a, b, c);
      drawPointToImage(image, startX+yC, startY-xC, a, b, c);
      drawPointToImage(image, startX+xC, startY-yC, a, b, c);
      drawPointToImage(image, startX-xC, startY-yC, a, b, c);
      drawPointToImage(image, startX-yC, startY-xC, a, b, c);
      drawPointToImage(image, startX-yC, startY+xC, a, b, c);
      drawPointToImage(image, startX-xC, startY+yC, a, b, c);
      
      if ( d < 0 ) 
      { 
        d = d+dx;  
        dx = dx+2; 
        dxy = dxy+2; 
        xC++;      
      }
      else 
      { 
        d = d+dxy; 
        dx = dx+2; 
        dxy = dxy+4; 
        xC++; 
        yC--;
      }
    }//end while
  }//end drawCircleToImage


  /*
  * Draws a axis parallel rectangle from x1, y1 to x2, y2 with a given color (a,b,c)
  */
  void static drawRectToImage
  (
    DrawingCanvas& image,
    const unsigned int& startX,
    const unsigned int& startY,
    const unsigned int& endX,
    const unsigned int& endY,
    const unsigned char& a,
    const unsigned char& b,
    const unsigned char& c
  )
  {
    unsigned int xstart, ystart, xend, yend;

    if (startX <= endX)
    {
      xstart = startX;
      xend   = endX;
    }
    else
    {
      xstart = endX;
      xend   = startX;
    }

    if (startY <= endY)
    {
      ystart = startY;
      yend   = endY;
    }
    else
    {
      ystart = endY;
      yend   = startY;
    }


    //if (x < image.resolutionWidth && y < image.resolutionHeight)
    
    if (ystart < image.height())
    {
      for (unsigned int i = xstart; i <= xend; i++)
      {
        if (i < image.width())
        {
          drawPointToImage(image, i, ystart, a, b, c);
          drawPointToImage(image, i, yend, a, b, c);
        }
      }
    }//end if

    if (xstart < image.width())
    {
      for (unsigned int i = ystart; i <= yend; i++)
      {
        if (i < image.height())
        {
          drawPointToImage(image, xstart, i, a, b, c);
          drawPointToImage(image, xend, i, a, b, c);
        }
      }
    }//end if
  }//end drawRectToImage


  /*
  * Draws a line from x1, y1 to x2, y2 with a given color (a,b,c)
  */
  void static drawLineToImage
  (
    DrawingCanvas& image,
    const unsigned int& startX,
    const unsigned int& startY,
    const unsigned int& endX,
    const unsigned int& endY,
    const unsigned char& a,
    const unsigned char& b,
    const unsigned char& c
  )
  {
    int xstart, ystart, xend, yend;

    if (startX <= endX)
    {
      xstart = (int)startX;
      ystart = (int)startY;
      xend   = (int)endX;
      yend   = (int)endY;
    }
    else
    {
      xstart = (int)endX;
      ystart = (int)endY;
      xend   = (int)startX;
      yend   = (int)startY;
    }

    int dx = xend-xstart;
    int dy = yend-ystart;

    int x = xstart;
    int y = ystart;

    if (abs(dx) >= abs(dy))  // Betrag von Anstieg <= 1
    {
      int fehler = (int)((double)(dx)/2); 

      if (dy >= 0)
      {
        while (x < xend)
        {
           x = x + 1;
           fehler = fehler-dy;
           if (fehler < 0) 
           {
            y = y + 1;
            fehler = fehler + dx;
           }
           drawPointToImage(image, (unsigned int)x, (unsigned int)y, a, b, c);
        }
      }
      else
      {
        while (x < xend)
        {
           x = x + 1;
           fehler = fehler+dy;
           if (fehler < 0) 
           {
            y = y - 1;
            fehler = fehler + dx;
           }
           drawPointToImage(image, (unsigned int)x, (unsigned int)y, a, b, c);
        }
      }
    }
    else
    //Anstiegsbetrag > 1
    {
      int fehler = (int)((double)(dy)/2); 

      if (dy >= 0)
      {
        while (y < yend)
        {
           y = y + 1;
           fehler = fehler-dx;
           if (fehler < 0) 
           {
            x = x + 1;
            fehler = fehler + dy;
           }
           drawPointToImage(image, (unsigned int)x, (unsigned int)y, a, b, c);
        }
      }
      else
      {
        while (y > yend)
        {
           y = y - 1;
           fehler = fehler-dx;
           if (fehler < 0) 
           {
            x = x + 1;
            fehler = fehler - dy;
           }
           drawPointToImage(image, (unsigned int)x, (unsigned int)y, a, b, c);
        }
      }
    }//end else
  }//end drawLineToImage


  /*
   * Colors point x,y with the given color (a,b,c)
   */
  inline static void drawPointToImage(
    DrawingCanvas& image, 
    const unsigned int& x, 
    const unsigned int& y,
    const unsigned char& a,
    const unsigned char& b,
    const unsigned char& c)
  { 
    if (x < image.width() && y < image.height())
    {
      image.drawPoint(x,y,a,b,c);
    }//end if
  }//end drawPointToImage



  //visualises the Coordinate System on the current image
  void static drawCoordinateSystemToImage(DrawingCanvas& image)
  {
    //int height = CameraInfo::resolutionWidth / 2;

    for( unsigned int i = 0; i < image.width() / 2; ++i)
    {
      image.drawPoint(i,5,0xFF,0x0,0x0);
    }//end for

    for( unsigned int i = 0; i < image.height() / 2; ++i)
    {
      image.drawPoint(5,i,0xFF,0x0,0x0);
    }//end for
  }//end drawCoordinateSystemToImage

};//end class ImageDrawings
}//end namespace naoth

#endif // __ImageDrawings_H_

