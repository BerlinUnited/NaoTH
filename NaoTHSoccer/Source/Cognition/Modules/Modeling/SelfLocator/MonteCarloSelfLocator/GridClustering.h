/**
* @file GridClustering.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class GridClustering
*/

#ifndef __GridClustering_h_
#define __GridClustering_h_

#include "SampleSet.h"

// debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Representations/Infrastructure/FieldInfo.h"

class GridClustering
{
public: 
  GridClustering(SampleSet& sampleSet);
  ~GridClustering() {}

  void cluster(double xPosOpponentGroundline, double yPosLeftGroundline);
  
private:
  SampleSet& sampleSet;

  class CountGrid2D
  {
  public:
    //IMPORTANT: At this moment GRID Cells have to be quadratic!!!
    //Field size: FieldInfo::xLength = 6000 and FieldInfo::yLength = 4000
    static const int GRID_X_PRECISION = 12;
    static const int GRID_Y_PRECISION = 8;
    static const int GRID_STEP = (6050)/GRID_X_PRECISION;

    double table[GRID_X_PRECISION][GRID_Y_PRECISION];

    void reset();
    Vector2<int> findBestCell();
  };

  CountGrid2D fieldGrid;
  CountGrid2D shiftedFieldGrid;

  void addPointToGrid(CountGrid2D& grid, const Vector2<double>& position,  double xPosOpponentGroundline, double yPosLeftGroundline);
  void draw_grid(const FieldInfo& fieldInfo);

};

#endif //__GridClustering_h_
