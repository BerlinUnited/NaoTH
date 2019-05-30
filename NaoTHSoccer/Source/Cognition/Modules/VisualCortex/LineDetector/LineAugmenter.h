#ifndef _LineAugmenter_H
#define _LineAugmenter_H

#include <ModuleFramework/Module.h>

#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/LinePercept2018.h"
#include "Representations/Perception/LinePerceptAugmented.h"

#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"

BEGIN_DECLARE_MODULE(LineAugmenter)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)

  REQUIRE(FieldInfo)

  REQUIRE(RansacCirclePercept2018)
  REQUIRE(RansacLinePercept)
  REQUIRE(ShortLinePercept)
  
  PROVIDE(LinePerceptAugmented)
END_DECLARE_MODULE(LineAugmenter)

class LineAugmenter: public LineAugmenterBase
{
public:
 LineAugmenter();
 ~LineAugmenter();

 virtual void execute();

private:
  void calculateCenterLine();

};

#endif // _LineAugmenter_H
