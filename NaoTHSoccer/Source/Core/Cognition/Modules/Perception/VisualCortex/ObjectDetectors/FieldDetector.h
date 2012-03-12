/* 
 * File:   FieldDetector.h
 * Author: Heinrich Mellmann
 *
 */

#ifndef __FieldDetector_H_
#define __FieldDetector_H_

#include <ModuleFramework/Module.h>

#include "Representations/Infrastructure/Image.h"
#include "Representations/Perception/FieldPercept.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Perception/ScanLineEdgelPercept.h"

BEGIN_DECLARE_MODULE(FieldDetector)
  REQUIRE(Image)
  REQUIRE(CameraMatrix)
  REQUIRE(ScanLineEdgelPercept)

  PROVIDE(FieldPercept)
END_DECLARE_MODULE(FieldDetector)


class FieldDetector : private FieldDetectorBase
{
public:
  FieldDetector();
  virtual ~FieldDetector();

  void execute();
};

#endif  /* __FieldDetector_H_ */

