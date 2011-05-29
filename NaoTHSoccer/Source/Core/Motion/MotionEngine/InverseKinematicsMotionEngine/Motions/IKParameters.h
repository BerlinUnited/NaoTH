/**
* @file IKParameters.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Declaration of parameters for IK motion
*/

#ifndef __IK__PARAMETERS_H_
#define __IK__PARAMETERS_H_

class IKParameters
{
public:
  struct {
    double speed;
  } stand;

  struct {
    double comHeight;
  } walk;

  double hipOffsetX;
  double footOffsetY;
  
  IKParameters();
};

#endif // __IK__PARAMETERS_H_