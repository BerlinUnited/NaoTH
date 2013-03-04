/**
* @file GridClustering.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class GridClustering
*/

#include "GridClustering.h"

  GridClustering::GridClustering(SampleSet& sampleSet)
  :
  sampleSet(sampleSet)
  {
    DEBUG_REQUEST_REGISTER("MCSL:draw_best_cells", "..", false);
  }


void GridClustering::cluster(double xPosOpponentGroundline, double yPosLeftSideline)
{
  fieldGrid.reset();
  shiftedFieldGrid.reset();

  for (unsigned int j = 0; j < sampleSet.size(); j++)
  {
    addPointToGrid(fieldGrid, sampleSet[j].translation, xPosOpponentGroundline, yPosLeftSideline);
    addPointToGrid(shiftedFieldGrid, sampleSet[j].translation, xPosOpponentGroundline, yPosLeftSideline);
  }//end for

  //Vector2<int> bestCell = fieldGrid.findBestCell();
  //Vector2<int> bestCellShifted = shiftedFieldGrid.findBestCell();

}//end cluster


void GridClustering::addPointToGrid(CountGrid2D& grid, const Vector2<double>& position, double xPosOpponentGroundline, double yPosLeftSideline)
{
  int x = (int)((position.x + xPosOpponentGroundline) / CountGrid2D::GRID_STEP);
  int y = (int)((position.y + yPosLeftSideline) / CountGrid2D::GRID_STEP); 

  //Check if x and y are within the array size and increment then
  if((x >= 0) && (y >= 0) &&
     (x < CountGrid2D::GRID_X_PRECISION) && (y < CountGrid2D::GRID_Y_PRECISION))
  {
    grid.table[x][y]++;
  }
}//end addPointToGrid


Vector2<int> GridClustering::CountGrid2D::findBestCell()
{
  Vector2<int> maxIndex;
  double maxValue = -1;

  for (int j = 0; j < GRID_X_PRECISION; j++)
  {  
    for (int i = 0; i < GRID_Y_PRECISION; i++)
    { 
      if(table[j][i] > maxValue)
      {
        maxValue = table[j][i];
        maxIndex.x = j;
        maxIndex.y = i;
      }//end if
    }//end for
  }//end for

  return maxIndex;
}//end findBestCell


void GridClustering::CountGrid2D::reset()
{
  for (int j = 0; j < GRID_X_PRECISION; j++)
  {  
    for (int i = 0; i < GRID_Y_PRECISION; i++)
    {
      table[j][i] = 0;
    }//end for
  }//end for
}//end reset


void GridClustering::draw_grid(const FieldInfo& fieldInfo)
{
  FIELD_DRAWING_CONTEXT;
  PEN("000000", 20);

  for (int x = 0; x <= fieldInfo.xLength; x+=CountGrid2D::GRID_STEP)
  {
    LINE(x - fieldInfo.xPosOpponentGroundline,
         0 - fieldInfo.yPosLeftSideline,
         x - fieldInfo.xPosOpponentGroundline,
         0 + fieldInfo.yPosLeftSideline);
  }

  for (int y = 0; y <= fieldInfo.yLength; y+=CountGrid2D::GRID_STEP)
  {
    LINE(0 - fieldInfo.xPosOpponentGroundline,
         y - fieldInfo.yPosLeftSideline,
         0 + fieldInfo.xPosOpponentGroundline,
         y - fieldInfo.yPosLeftSideline);
  }


  DEBUG_REQUEST("MCSL:draw_best_cells",
    for (int j = 0; j < CountGrid2D::GRID_X_PRECISION; j++)
    {  
      for (int i = 0; i < CountGrid2D::GRID_Y_PRECISION; i++)
      { 
        if (fieldGrid.table[j][i] > (int)(sampleSet.size()*0.03))
        {
          FIELD_DRAWING_CONTEXT;
          //Draw best grid cells
          if (fieldGrid.table[j][i] >= (int)(sampleSet.size()*0.09))
          {
            PEN("FF0000", 20);
          }
          else if (fieldGrid.table[j][i] > (int)(sampleSet.size()*0.06))
          {
            PEN("FF5555", 20);
          }
          else if (fieldGrid.table[j][i] > (int)(sampleSet.size()*0.03))
          {
            PEN("FFAAAA", 20);
          }

          CIRCLE(j*CountGrid2D::GRID_STEP-fieldInfo.xPosOpponentGroundline + CountGrid2D::GRID_STEP/2,
                 i*CountGrid2D::GRID_STEP-fieldInfo.yPosLeftSideline  + CountGrid2D::GRID_STEP/2,
                 CountGrid2D::GRID_STEP/2);
        }//end if
      }//end for
    }//end for
   );
}//end draw_grid
