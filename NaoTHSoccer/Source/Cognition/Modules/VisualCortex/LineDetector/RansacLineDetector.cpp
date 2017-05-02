#include "RansacLineDetector.h"

RansacLineDetector::RansacLineDetector()
{
  // initialize some stuff here
  DEBUG_REQUEST_REGISTER("Vision:RansacLineDetector:edgel_pairs", "", false);
}

RansacLineDetector::~RansacLineDetector()
{
  // clean some stuff here
}

void RansacLineDetector::execute()
{
  // do some stuff here

  DEBUG_REQUEST("Vision:RansacLineDetector:edgel_pairs",
    CANVAS("ImageTop");

    for(size_t i = 0; i < getScanLineEdgelPerceptTop().pairs.size(); i++)
    {
      const ScanLineEdgelPercept::EdgelPair& pair = getScanLineEdgelPerceptTop().pairs[i];
      const Edgel& edgelOne = getScanLineEdgelPerceptTop().edgels[pair.begin];
      const Edgel& edgelTwo = getScanLineEdgelPerceptTop().edgels[pair.end];

      PEN("000000",0.1);
      //LINE_PX(ColorClasses::red, edgelOne.center.x, edgelOne.center.y, edgelTwo.center.x, edgelTwo.center.y);
      LINE(edgelOne.point.x, edgelOne.point.y, edgelTwo.point.x, edgelTwo.point.y);
      PEN("FF0000",0.1);
      CIRCLE( edgelOne.point.x, edgelOne.point.y, 3);
      PEN("0000FF",0.1);
      CIRCLE( edgelTwo.point.x, edgelTwo.point.y, 2);
    }
  );
}
