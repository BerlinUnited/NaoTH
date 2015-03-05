/**
* @file VoronoiBasedSituationMapProvider.h
*/

#ifndef _VoronoiBasedSituationMapProvider_H
#define _VoronoiBasedSituationMapProvider_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>

#include "Representations/Infrastructure/FieldInfo.h"
#include "Tools/VoronoiCellGraph/Pattern.h"
#include "Tools/VoronoiCellGraph/VoronoiCellGraph.h"

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugModify.h"

BEGIN_DECLARE_MODULE(VoronoiBasedSituationMapProvider)
  REQUIRE(FieldInfo)
  PROVIDE(DebugRequest)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugModify)
END_DECLARE_MODULE(VoronoiBasedSituationMapProvider)

class VoronoiBasedSituationMapProvider: public VoronoiBasedSituationMapProviderBase
{
public:
 VoronoiBasedSituationMapProvider();
 ~VoronoiBasedSituationMapProvider();

 virtual void execute();
 
private:

	//patterns
  Pattern backgroundpattern;
  VoronoiCellGraph voronoiCellGraph;
  void doDebugRequests();

};

#endif  /* _VoronoiBasedSituationMapProvider_H */