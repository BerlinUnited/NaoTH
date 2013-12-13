/**
* @file GridCreator.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class GridCreator
*/

#ifndef __GridCreator_h_
#define __GridCreator_h_

//#include <cstddef>
#include <Tools/Math/Vector2.h>
#include <Tools/Math/Common.h>

#include "Grid.h"
#include "UniformGrid.h"

/*
  GridCreator
  enthaelt Methoden fuer die Gitter-Generierung.
*/
class GridCreator
{
  public:
    // generiert ein uniformes quadratisches Gitter mit gridWidth Spalten und gridHeight Zeilen
    static void createUniformGrid(UniformGrid& uniformGrid, const int imageWidth, const int imageHeight, const int gridWidth, const int gridHeight);
    static void createProbabilisticGrid(Grid& grid, const int imageWidth, const int imageHeight,  const int sampleNumber);
};
#endif// __GridCreator_h_
