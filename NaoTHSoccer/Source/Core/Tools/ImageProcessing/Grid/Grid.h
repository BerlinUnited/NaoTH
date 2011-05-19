#ifndef __Grid_H_
#define __Grid_H_

#include <Tools/Math/Vector2.h>
#include <Tools/Debug/NaoTHAssert.h>

/**
 * class Grid
 * repraesentiert ein Gitter im Bild durch ein Feld von Punkten, dabei
 * werden nur die Koordinaten der Punkte im Bild gespeichert (keine Farbe etc.)                         
 */
class Grid
{
private:
  unsigned int _numberOfGridPoints;

public:
  Grid(unsigned int maxNumberOfPoints)
    : 
    _numberOfGridPoints(0),
    maxNumberOfPoints(maxNumberOfPoints),
    numberOfGridPoints(_numberOfGridPoints)
  {
    pointsCoordinates = new Vector2<int>[maxNumberOfPoints];
  }//end constructor Grid

  virtual ~Grid()
  {
    delete[] pointsCoordinates;
  }

  // HACK: remove the constants
  const static unsigned int HACK_MAX_WIDTH = 640;
	// HACK: remove the constants
  const static unsigned int HACK_MAX_HEIGHT = 480;

  const unsigned int maxNumberOfPoints;
  const unsigned int& numberOfGridPoints;

  unsigned int addPoint(const Vector2<int>& point)
  {
    ASSERT(numberOfGridPoints < maxNumberOfPoints);

    pointsCoordinates[numberOfGridPoints] = point;
    _numberOfGridPoints++;
    return numberOfGridPoints-1;
  }//end addPoint

  const Vector2<int>& getPoint(const unsigned int& i) const
  {
    ASSERT(i < maxNumberOfPoints);
    return pointsCoordinates[i];
  }//end getPoint

private:
  // Disable copy constructor and assignment operator.
  Grid(const Grid& other);
  Grid& operator=(const Grid& other);

  Vector2<int>* pointsCoordinates;
};//end class Grid

#endif // __Grid_H__

