/**
 * @file ActiveGoalLocator.h
 *
 * @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 * Implementation of class FieldCompass
 */

#include "FieldCompass.h"

FieldCompass::FieldCompass()
{
}

void FieldCompass::execute()
{
  getCompassDirection().angle = getRobotPose().rotation;
}
