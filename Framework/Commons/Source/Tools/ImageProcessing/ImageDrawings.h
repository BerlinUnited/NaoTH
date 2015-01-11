/**
* @file ImageDrawings.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class ImageDrawings
*
* TODO: how to use
* ...
*/
#ifndef _ImageDrawings_H_
#define _ImageDrawings_H_

#include <Representations/Infrastructure/Image.h>
#include <stdlib.h>

namespace naoth
{

class DrawingCanvas
{
public:
  virtual void drawPoint
  (
    const int x,
    const int y,
    const unsigned char a,
    const unsigned char b,
    const unsigned char c
  ) = 0;

  virtual ~DrawingCanvas(){}

  virtual unsigned int width() const = 0;
  virtual unsigned int height() const = 0;
};

class ImageDrawingCanvas : public DrawingCanvas
{
private:
  Image& image; // NOTE: because of this ImageDrawingCanvas cannot be copied
public:
  ImageDrawingCanvas(Image& image) : image(image) {}

  virtual void drawPoint
    (
      const int x,
      const int y,
      const unsigned char a,
      const unsigned char b,
      const unsigned char c
    ) {
      image.set(x,y, a, b, c);
    }

  virtual unsigned int width() const { return image.width(); }
  virtual unsigned int height() const { return image.height(); }
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
    const int startX,
    const int startY,
    const int radius,
    const unsigned char a,
    const unsigned char b,
    const unsigned char c
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
    const int startX,
    const int startY,
    const int endX,
    const int endY,
    const unsigned char a,
    const unsigned char b,
    const unsigned char c
  )
  {
    int xstart, ystart, xend, yend;

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
    
    if (ystart < (int)image.height())
    {
      for (int i = xstart; i <= xend; i++)
      {
        if (i < (int)image.width())
        {
          drawPointToImage(image, i, ystart, a, b, c);
          drawPointToImage(image, i, yend, a, b, c);
        }
      }
    }//end if

    if (xstart < (int)image.width())
    {
      for (int i = ystart; i <= yend; i++)
      {
        if (i < (int)image.height())
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
    const int startX,
    const int startY,
    const int endX,
    const int endY,
    const unsigned char a,
    const unsigned char b,
    const unsigned char c
  )
  {
    int xstart, ystart, xend, yend;

    if (startX <= endX)
    {
      xstart = startX;
      ystart = startY;
      xend   = endX;
      yend   = endY;
    }
    else
    {
      xstart = endX;
      ystart = endY;
      xend   = startX;
      yend   = startY;
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
           drawPointToImage(image, x, y, a, b, c);
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
           drawPointToImage(image, x, y, a, b, c);
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
           drawPointToImage(image, x, y, a, b, c);
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
           drawPointToImage(image, x, y, a, b, c);
        }
      }
    }//end else
  }//end drawLineToImage


  /*
   * Colors point x,y with the given color (a,b,c)
   */
  inline static void drawPointToImage(
    DrawingCanvas& canvas, 
    const int x, 
    const int y,
    const unsigned char a,
    const unsigned char b,
    const unsigned char c)
  { 
    if (x >= 0 && y >=0 && x < (int)canvas.width() && y < (int)canvas.height())
    {
      canvas.drawPoint(x,y,a,b,c);
    }//end if
  }//end drawPointToImage



  //visualises the Coordinate System on the current image
  void static drawCoordinateSystemToImage(DrawingCanvas& image)
  {
    for( int i = 0; i < (int)image.width() / 2; ++i) {
      image.drawPoint(i,5,0xFF,0x0,0x0);
    }

    for( int i = 0; i < (int)image.height() / 2; ++i) {
      image.drawPoint(5,i,0xFF,0x0,0x0);
    }
  }

};//end class ImageDrawings
}//end namespace naoth

#endif // _ImageDrawings_H_

