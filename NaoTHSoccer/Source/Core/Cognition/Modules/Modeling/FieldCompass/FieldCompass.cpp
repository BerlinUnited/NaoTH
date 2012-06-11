/**
 * @file ActiveGoalLocator.h
 *
 * @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 * Implementation of class FieldCompass
 */

#include "FieldCompass.h"

FieldCompass::FieldCompass()
{
}//Constructor

void FieldCompass::execute()
{
  getCompassDirection().angle = getRobotPose().rotation;
}//end FieldCompass::execute()
