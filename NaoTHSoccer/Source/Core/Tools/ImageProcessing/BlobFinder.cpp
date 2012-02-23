
/**
* @file BlobFinder.cpp
*
* Implementation of class BlobFinder
*
*/


#include "BlobFinder.h"

// common tools
#include <Tools/Debug/DebugImageDrawings.h>

#include "Tools/Debug/DebugRequest.h"
#include "Tools/DataStructures/ArrayQueue.h"



BlobFinder::BlobFinder(const ColoredGrid& coloredGrid)
  : coloredGrid(coloredGrid), 
    visitedPixels(coloredGrid.uniformGrid.width, coloredGrid.uniformGrid.height)
{
  DEBUG_REQUEST_REGISTER("BlobFinder:mark_blobs", "mark found blobs on the image", false);
}



void BlobFinder::execute(Container<Blob>& result, 
                         bool connectedColors[ColorClasses::numOfColors], 
                         const Area<int>& searchArea)
{
  // mask for the search neighborhood
  static const Vector2<int> mask[8] = 
  {
    Vector2<int>( 0, 1),
    Vector2<int>( 1, 0),
    Vector2<int>( 0,-1),
    Vector2<int>(-1, 0),
    Vector2<int>( 1, 1),
    Vector2<int>(-1,-1),
    Vector2<int>(-1, 1),
    Vector2<int>( 1,-1)
  };

  //get width and height of the scaled image (uniformgrid)
  const int gridWidth =coloredGrid.uniformGrid.width;
  const int gridHeight=coloredGrid.uniformGrid.height;

  //remember all the pixels that were visited
  //and reset them first
  visitedPixels.reset();

  // TODO: it maybe slow to create the ArrayQueue every time
  //to store the pixels still to check
  static ArrayQueue<int> pixelsToCheck;
  pixelsToCheck.clear();
  
  //to store the drawingcolor for debugDrawings
  ColorClasses::Color drawingColor = ColorClasses::green;

  //go through all colors
  for(unsigned int color = 0; color < ColorClasses::numOfColors; color++)
  {
    //check if there are pixels of this color and the color is relevant for blob search
    if(coloredGrid.numberOfColorPoints[color] == 0 || !connectedColors[color])
      continue;

    //go through all pixels of this color
    for(unsigned int startingPointIndex=0; startingPointIndex < coloredGrid.numberOfColorPoints[color]; startingPointIndex++)
    {
      int pixelIndex = coloredGrid.colorPointsIndex[color][startingPointIndex]; //(the index)
      Vector2<int> startingPixel = coloredGrid.getColorPoint(pixelIndex);//uniformGrid.gridCoordinates[pixelIndex];

      //check if this point has been visited
      if(visitedPixels(startingPixel.x, startingPixel.y))
        continue;

      //...and push them into the todo list
      pixelsToCheck.push(pixelIndex);

      //init a new blob creator
      BlobCreator blobCreator;

      //while there are pixels to check
      while(!pixelsToCheck.empty())
      {
        //... take the first of them (the index)
        int pixelIndex = pixelsToCheck.front();
        pixelsToCheck.pop();
        
        // (pixelIndex)
        //get the GRID coordinates of pixel
        const Vector2<int>& pixel = coloredGrid.getColorPoint(pixelIndex);//.uniformGrid.gridCoordinates[pixelIndex];

        //check if the pixel wasn't treated allready and is of a relevant color
        if(visitedPixels(pixel.x, pixel.y) ||
           !connectedColors[coloredGrid.pointsColors[pixelIndex]]) continue;

        // (pixelIndex)
        //get the coordinates of pixel in the real image (as opposed to grid coordinates)
        const Vector2<int>& pixelInImage = coloredGrid.getImagePoint(pixelIndex);//uniformGrid.pointsCoordinates[pixelIndex];

        //check if the pixel is inside the searchArea
        //TODO: give the Area in image coordinates
        if(!searchArea.isInside(coloredGrid.getImagePoint(pixel))) continue;

           
        //the pixel is visited now
        visitedPixels.set(pixel.x, pixel.y, true);

        // add the pixel to the blob
        blobCreator.addPoint(pixelInImage);

        //check the neighboring pixels
        for(int i = 0; i < 8; i++)
        {
          Vector2<int> neighborPixel = pixel + mask[i];
          // check if the pixel is inside the grid
          // we have to ckeck it here, because in the case the pixel is 
          // outside the grid its index in scaledImageIndex is not defined
          if(neighborPixel.x >= 0 && neighborPixel.x < gridWidth && 
             neighborPixel.y >= 0 && neighborPixel.y < gridHeight)
          {
            pixelsToCheck.push(coloredGrid.getScaledImageIndex(neighborPixel));
          }
        }//end for

        //mark the pixel in the image if requested
        DEBUG_REQUEST("BlobFinder:mark_blobs",
          POINT_PX(drawingColor, (unsigned int)(pixel.x), (unsigned int)(pixel.y));
        );//end DEBUG
      }//end while

      //if the blob consists of at least one pixel
      if(blobCreator.moments.getRawMoment(0, 0) > 0)
      {        
        const Blob& blob = blobCreator.createBlob();

        //draw boundaries and center of mass
        DEBUG_REQUEST("BlobFinder:mark_blobs",
          RECT_PX(drawingColor,
                  (unsigned int)(blob.upperLeft.x),
                  (unsigned int)(blob.upperLeft.y),
                  (unsigned int)(blob.lowerRight.x),
                  (unsigned int)(blob.lowerRight.y));
          CIRCLE_PX(drawingColor, blob.centerOfMass.x, blob.centerOfMass.y, 4);
        );//end DEBUG

        // save the blob
        result.add(blob);
        
        //cycle DrawingColor
        drawingColor=(ColorClasses::Color)((drawingColor+1)%ColorClasses::numOfColors);
      }//end if

    }//end for startingPointIndex
  }//end for color
}//end execute


Blob BlobFinder::regionGrowExpandArea(
                          const Vector2<int>& startingPoint, 
                          ColorClasses::Color color)
{

//  const int numberOfDirections = 8;
  Vector2<int> mask[] = 
  {
    Vector2<int>(-1,  0),
    Vector2<int>(-1, -1),
    Vector2<int>( 0, -1),
    Vector2<int>( 1, -1),
    Vector2<int>( 1,  0),
    Vector2<int>( 1,  1),
    Vector2<int>( 0,  1),
    Vector2<int>(-1,  1)
  };

  //get width and height of the scaled image (uniformgrid)
  const int gridWidth =coloredGrid.uniformGrid.width; 
  const int gridHeight=coloredGrid.uniformGrid.height;

  //remember all the pixels that were visited
  //and reset them first
  visitedPixels.reset();

  //init a new blob creator
  BlobCreator blobCreator;


  std::list<int> open;
  
  if(startingPoint.x >= 0 && startingPoint.x < gridWidth && 
     startingPoint.y >= 0 && startingPoint.y < gridHeight)
  {
    int index = coloredGrid.getScaledImageIndex(startingPoint);
    if(coloredGrid.pointsColors[index] == color)
      open.push_back(index);
  }

  while(!open.empty())
  {
    // TODO: check if a reference can be used
    int pixelIndex = open.front();
    open.pop_front();

    // (pixelIndex)
    //get the GRID coordinates of pixel
    const Vector2<int>& pixel = coloredGrid.getColorPoint(pixelIndex);//.uniformGrid.gridCoordinates[pixelIndex];

    //check if the pixel wasn't treated allready and is of a relevant color
    if(visitedPixels(pixel.x, pixel.y) ||
       coloredGrid.pointsColors[pixelIndex] != color) continue; 

    // (pixelIndex)
    //get the coordinates of pixel in the real image (as opposed to grid coordinates)
    const Vector2<int>& pixelInImage = coloredGrid.getImagePoint(pixelIndex);// .uniformGrid.pointsCoordinates[pixelIndex];

    //check if the pixel is inside the searchArea
    //TODO: give the Area in image coordinates
    //if(!searchArea.isInside(pixel)) continue;
           
    //the pixel is visited now
    visitedPixels.set(pixel.x, pixel.y, true);

    // add the pixel to the blob
    blobCreator.addPoint(pixelInImage);
    
    //check the neighboring pixels
    for(int i = 0; i < 8; i++)
    {
      Vector2<int> neighborPixel = pixel + mask[i];
      // check if the pixel is inside the grid
      // we have to ckeck it here, because in the case the pixel is 
      // outside the grid its index in scaledImageIndex is not defined
      if(neighborPixel.x >= 0 && neighborPixel.x < gridWidth && 
         neighborPixel.y >= 0 && neighborPixel.y < gridHeight)
      {
        open.push_back(coloredGrid.getScaledImageIndex(neighborPixel));
      }
    }//end for
  }//end while
 
  const Blob& blob = blobCreator.createBlob();

  //draw boundaries and center of mass
  DEBUG_REQUEST("ImageProcessor:BlobFinder:mark_blobs",
    RECT_PX(color,
            (unsigned int)(blob.upperLeft.x),
            (unsigned int)(blob.upperLeft.y),
            (unsigned int)(blob.lowerRight.x),
            (unsigned int)(blob.lowerRight.y));
    CIRCLE_PX(color, blob.centerOfMass.x, blob.centerOfMass.y, 4);
  );//end DEBUG

  return blob;

}//end expandArea
