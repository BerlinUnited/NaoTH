/**
* @file GridProvider.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Declaration of class GridProvider
*/

#ifndef __GridProvider_h_
#define __GridProvider_h_

#include <ModuleFramework/Module.h>

// Debug
#include "Core/Tools/Debug/DebugRequest.h"
//#include "Tools/Debug/DebugImageDrawings.h"
#include <Tools/Debug/Stopwatch.h>

//Tools
#include "Core/Tools/ImageProcessing/ColoredGrid.h"
#include "Core/Tools/ImageProcessing/Histogram.h"

// Representations
#include "Representations/Infrastructure/Image.h"
#include "Core/Representations/Infrastructure/ColorTable64.h"

//////////////////// BEGIN MODULE INTERFACE DECLARATION ////////////////////

BEGIN_DECLARE_MODULE(GridProvider)
  REQUIRE(Image)
  PROVIDE(ColorTable64)

  PROVIDE(ColoredGrid)
  PROVIDE(Histogram)
END_DECLARE_MODULE(GridProvider)

//////////////////// END MODULE INTERFACE DECLARATION //////////////////////

class GridProvider : public GridProviderBase
{
public:

  GridProvider();
  ~GridProvider(){}


  /** executes the module */
  void execute();

  void createColorTable() const;

private:
  void calculateColoredGrid();
};

#endif //__GridProvider_h_
