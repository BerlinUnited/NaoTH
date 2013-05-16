/**
* @file FootGroundContactDetector.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* detect if the foot touch the ground
*/

#ifndef _FootGroundContactDetector_H
#define _FootGroundContactDetector_H


#include <ModuleFramework/Module.h>

// representations
#include <Representations/Infrastructure/FSRData.h>
#include "Representations/Modeling/GroundContactModel.h"

// tools
#include "Tools/LinearClassifier.h"
#include "Tools/DataStructures/RingBufferWithSum.h"


BEGIN_DECLARE_MODULE(FootGroundContactDetector)
  PROVIDE(FSRData)
  PROVIDE(GroundContactModel)
END_DECLARE_MODULE(FootGroundContactDetector)


class FootGroundContactDetector: private FootGroundContactDetectorBase
{
public:
  FootGroundContactDetector();
  ~FootGroundContactDetector();

  void execute();

private:

  RingBufferWithSum<double, 100> leftFSRBuffer;
  RingBufferWithSum<double, 100> rightFSRBuffer;

};

#endif  /* _FootGroundContactDetector_H */

