/**
* @file ImageDrawings.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class ImageDrawings
*/
#ifndef __ImageDrawings_H_
#define __ImageDrawings_H_

#include "Tools/ColorClasses.h"
#include "ColorModelConversions.h"
#include <stdlib.h>

namespace naoth
{
  
class DrawingCanvas
{
public:
  virtual void drawPoint(
    unsigned int x, 
    unsigned int y,
    unsigned char a,
    unsigned char b,
    unsigned char c) = 0;

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
   * @param color Color of the Circle
  */
  void static drawCircleToImage(
    DrawingCanvas& image, 
    ColorClasses::Color color, 
    unsigned int startX, 
    unsigned int startY, 
    unsigned int radius)
  {
    int xC = 0; 
    int yC = radius; 
    int d = 1-radius;
    int dx = 3; 
    int dxy = -2*radius+5;

    while ( yC >= xC )
    {
      drawPointToImage(image, color, startX+xC, startY+yC);
      drawPointToImage(image, color, startX+yC, startY+xC);
      drawPointToImage(image, color, startX+yC, startY-xC);
      drawPointToImage(image, color, startX+xC, startY-yC);
      drawPointToImage(image, color, startX-xC, startY-yC);
      drawPointToImage(image, color, startX-yC, startY-xC);
      drawPointToImage(image, color, startX-yC, startY+xC);
      drawPointToImage(image, color, startX-xC, startY+yC);
      
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
  * Draws a axis parallel rectangle from x1, y1 to x2, y2 with a given color
  */
  void static drawRectToImage(
    DrawingCanvas& image, 
    ColorClasses::Color color, 
    unsigned int startX, 
    unsigned int startY, 
    unsigned int endX, 
    unsigned int endY )
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
				  drawPointToImage(image, color, i, ystart);
          drawPointToImage(image, color, i, yend);
        }
		  }
	  }//end if

	  if (xstart < image.width())
	  {
		  for (unsigned int i = ystart; i <= yend; i++)
		  {
			  if (i < image.height())
			  {
          drawPointToImage(image, color, xstart, i);
          drawPointToImage(image, color, xend, i);
		    }
		  }
	  }//end if
  }//end drawRectToImage


  /*
  * Draws a line from x1, y1 to x2, y2 with a given color
  */
  void static drawLineToImage(
    DrawingCanvas& image, 
    ColorClasses::Color color, 
    unsigned int startX, 
    unsigned int startY, 
    unsigned int endX, 
    unsigned int endY )
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
			     drawPointToImage(image, color, (unsigned int)x, (unsigned int)y);
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
			     drawPointToImage(image, color, (unsigned int)x, (unsigned int)y);
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
			     drawPointToImage(image, color, (unsigned int)x, (unsigned int)y);
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
			     drawPointToImage(image, color, (unsigned int)x, (unsigned int)y);
			  }
		  }
    }//end else
  }//end drawLineToImage



  /*
   * Colors point x,y with the given Color
   */
  void static drawPointToImage(
    DrawingCanvas& image, 
    ColorClasses::Color color, 
    unsigned int x, 
    unsigned int y)
  { 
    if ((x < image.width()) && 
        (y < image.height()))
    {
      unsigned char a;
      unsigned char b;
      unsigned char c;

      unsigned char yy;
      unsigned char cb;
      unsigned char cr;

      ColorClasses::colorClassToRGB(color, a, b, c);
      ColorModelConversions::fromRGBToYCbCr(a,b,c,yy,cb,cr);
      image.drawPoint(x,y,yy,cb,cr);
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

}

#endif // __ImageDrawings_H_

