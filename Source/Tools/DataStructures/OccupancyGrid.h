/**
* @file OccupancyGrid.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration and definition of class OccupancyGrid. OccupancyGrid implements an
* two dimensional boolean array. The main feature of te class is a very fast 
* reset method which resets the whole array to false. Calling the reset() method 
* will not reset every value to false, but add a bias to the value which is interpreted
* as true, e.g., at the beginning is false = 0 and true = 1 after reset we get
* false = 0,1 and true = 2, i.e., every value less then true is interpreted as false.
* Here is an example of usage in comarison with a regular boolean array:

  // define
  OccupancyGrid<5,5> occupancyGrid;
  bool test[5][5];

  for(long k = 0; k < 1000000; k++)
  {
    // reset
    for(int x = 0; x < 5; x++)
        for(int y = 0; y < 5; y++)
          test[x][y] = false;

    occupancyGrid.reset();

    // set random values
    for(int i = 0; i < 5; i++)
    {
      int x = Math::random(0,4);
      int y = Math::random(0,4);

      test[x][y] = true;
      occupancyGrid.set(x,y,true);
    }//end for


    // compare
    for(int x = 0; x < 5; x++)
      for(int y = 0; y < 5; y++)
        if(occupancyGrid(x,y) != test[x][y])
          cout << "error" << endl;

  }//end for k


* Remark: the class ExtendedOccupancyGrid extends OccupancyGrid with 
* the non const ()-operator, i.e., you may write something like 

  ExtendedOccupancyGrid<5,5> occupancyGrid;

  // following is equivalent
  occupancyGrid.set(x,y,true);
  occupancyGrid(x,y) = true;

* nevertheless, this operator () may be more costly than the inline method set.
* This has not been tested till now...
*
* CAUTION: at the cunstruction time the whole array is resetted once to 0
* this can consume some time, so DON'T use the OccupancyGrid as a local variable!!!
*/

#ifndef __OccupancyGrid_h__
#define __OccupancyGrid_h__

#include "Tools/Debug/NaoTHAssert.h"

class OccupancyGrid
{

public:
  const unsigned int width;
  const unsigned int height;

  OccupancyGrid(int width, int height)
    : width(width),
      height(height)
  { 
    field = new long[width*height];
    hardReset();
  }

  virtual ~OccupancyGrid()
  {
    delete[] field;
  }
  
  void reset()
  {

    toResetX++;
    if(toResetX == width)
    {
      toResetX = 0;
      toResetY++;
    }//end if

    // the whole field has been iterated
    // switch to another bias
    if(toResetY == height)
    {
      toResetY = 0;
      bias = 1 - bias; // bias = !bias :)
    }

    currentValue = bias*height*width + (toResetY*width + toResetX + 1);

    // reset a single element
    field[toResetY*width+toResetX] = 0;
  }//end reset

  inline bool operator() (unsigned x, unsigned y) const
  {
    ASSERT(x < width && y < height);
    return field[y*width+x] == currentValue;
  }

  inline bool get(unsigned x, unsigned y) const
  {
    ASSERT(x < width && y < height);
    return field[y*width+x] == currentValue;
  }

  inline void set(unsigned x, unsigned y, bool value)
  {
    ASSERT(x < width && y < height);
    field[y*width+x] = value?currentValue:0;
  }
  
protected:

  // TODO: use memset?
  void hardReset()
  {
    for(unsigned int i = 0; i < height*width; i++)
    {
      field[i] = 0;
    }//end for

    currentValue = 1;
    toResetX = 0;
    toResetY = 0;
    bias = 0;
  }//end hardReset

  /** describes the value which is interpreted as "true" :) */
  long currentValue;

  long bias;

  unsigned int toResetX;
  unsigned int toResetY;

  // TODO: long long?
  long* field;
};


class ExtendedOccupancyGrid: public OccupancyGrid
{

public:
  ExtendedOccupancyGrid(int width, int height)
    : OccupancyGrid(width, height)
  {
  }

  virtual ~ExtendedOccupancyGrid(){}

  /**
  * wrapper for the function set(x,y,value) in OccupancyGrid
  */
  class ValueAccessor
  {
    int x;
    int y;
    OccupancyGrid* grid;

    ValueAccessor& create(int x, int y, OccupancyGrid* grid)
    {
      this->x = x;
      this->y = y;
      this->grid = grid;
      return *this;
    }

    ValueAccessor(){}
    ~ValueAccessor(){}

  public:
    ValueAccessor& operator= (const bool& v)
    {
      grid->set(x,y,v);
      return *this;
    }

    operator bool()
    {
      return grid->get(x,y);
    }

    friend class ExtendedOccupancyGrid;
  };

  ValueAccessor& operator() (unsigned x, unsigned y)
  {
    return valueAccessor.create(x,y,this);
  }

private:
  ValueAccessor valueAccessor;

};

#endif //__OccupancyGrid_h__
