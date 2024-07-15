#include "PoseDetector.h"

PoseDetector::PoseDetector()
{
  getDebugParameterList().add(&params);
}

PoseDetector::~PoseDetector() {
  getDebugParameterList().remove(&params);
}