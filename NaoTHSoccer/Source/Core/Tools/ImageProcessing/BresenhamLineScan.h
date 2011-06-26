/**
* @file Tools/ImageProcessing/BresenhamLineScan.h
* 
* Utility class which performs the Bresenham algorithm for line scanning.
* Some constructors set numberOfPixels which can be used as a termination condition to 
* prevent scans outside the image boundaries.
*
* @author <a href="mailto:timlaue@tzi.de">Tim Laue</a>
* @author <a href="mailto:walter.nistico@uni-dortmund.de">Walter Nistico</a>
* @author <a href="mailto:oberlies@sim.tu-darmstadt.de">Tobias Oberlies</a> (revised constructors and commenting) 
*/

#ifndef __BresenhamLineScan_h_
#define __BresenhamLineScan_h_

//#include "Tools/CameraGeometry.h"
#include "Tools/Math/Common.h"
#include "Tools/Math/Line.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Math/MVTools.h"

#include "Representations/Infrastructure/CameraInfo.h"

class BresenhamLineScan
{
public:
	/** 
	 * Constructs a scanline through the two points. numberOfPixels can be used.
	 * @param start The start point of the line segment.
	 * @param end The end point of the line segment.
	 */
	BresenhamLineScan(const Vector2<int>& start, const Vector2<int>& end);

	/**
	 * Constructs a scanline with the given direction.
	 * @param direction The direction vector of the scanline.
	 */
	BresenhamLineScan(const Vector2<double>& direction);

	/**
	 * Constructs a scanline with the given direction.
	 * @param direction The direction (angle) of the line, expressed in radians.
	 */
	BresenhamLineScan(const double& direction);

	/**
	 * Constructs a scanline with the given direction starting at start and ending at the 
	 * image boundary. numberOfPixels can be used.
	 * @param start The start point of the line.
	 * @param direction The direction (angle) of the line, expressed in radians.
	 * @param cameraInfo The cameraInfo object of the camera that captured the image.
	 */
  BresenhamLineScan(const Vector2<int>& start, const double& direction, const naoth::CameraInfo& cameraInfo);
  
	/** 
	 * Constructs a scanline with the given direction starting at start and ending at the 
	 * image boundary. numberOfPixels can be used.
	 * @param start The start point of the ray
	 * @param direction The vector pointing in the direction of scanning. Must not be a null 
	 *        vector.
	 * @param cameraInfo The cameraInfo object of the camera that captured the image.
	 * @author Tobias Oberlies
	 */
	BresenhamLineScan(const Vector2<int>& start, const Vector2<double>& direction, const naoth::CameraInfo& cameraInfo);

	BresenhamLineScan(const Math::Line& line, const naoth::CameraInfo& cameraInfo);
  
  void setup(const Vector2<int>& start, const Vector2<int>& end);
  void setup(const double& direction);
  void setup(const Vector2<double>& direction);
	void setup(const Vector2<int>& start, const double& direction, const naoth::CameraInfo& cameraInfo);
	void setup(const Vector2<int>& start, const Vector2<double>& direction, const naoth::CameraInfo& cameraInfo);
	void setup(const Math::Line& line, const naoth::CameraInfo& cameraInfo);


	/** Resets the error counter */
	inline void init()
	{
		error = baseError;
	}

	/**
	 * Increments the coordinates to the next point on the line.
	 * @param pos The position of the current pixel on the line, which is incremented by the
	 * Bresenham algorithm
	 */
	inline void getNext(Vector2<int>& pos)
	{
	  pos += standardOffset;
	  error += delta;
	  if(error > 0)
	  {
		  pos += correctionOffset;
		  error += resetError;
	  }
    pixelCount++;
	}//end getNext

  inline bool getNextWithCheck(Vector2<int>& pos)
	{
    if (pixelCount >= 0 && pixelCount < numberOfPixels)
    {
      getNext(pos);
      return true;
    }
	  return false;
	}//end getNextWithCheck

	/**
	 * Increments the coordinates to the next point on the line.
	 * @param pos The position of the current pixel on the line, which is incremented by the
	 * Bresenham algorithm
	 */
	inline bool getLast(Vector2<int>& pos)
	{
    bool valid = (pixelCount > 0 && pixelCount <= numberOfPixels);
	  if(error - delta > 0)
	  {
		  pos -= correctionOffset;
		  error -= resetError;
	  }
	  pos -= standardOffset;
	  error -= delta;
    pixelCount--;
    return valid;
	}//end getNext

	/**
	 * In conjuction with certain constructors (see above), this value can be used as a 
	 * termination criterion for the scan. In those cases, getNext can be called 
	 * numberOfPixels times without leaving the image boundaries.
	 */
	int numberOfPixels;
  int pixelCount;

private:
	/** Increase x-values, if true. */
	bool alongX;
	/** The error per step. */
	int delta;
	/** The initial error value. */
	int baseError;
	/** Resets the error to a value less than zero. */
	int resetError;
	/** The standard offset per step. */
	Vector2<int> standardOffset;
	/** The additional offset, if the error is above zero. */
	Vector2<int> correctionOffset;
	/** The current error counter. */
	int error;

	/** Computes the Bresenham parameters. */
	void setup(const Vector2<int>& diff);

};


#endif //__BresenhamLineScan_h_
