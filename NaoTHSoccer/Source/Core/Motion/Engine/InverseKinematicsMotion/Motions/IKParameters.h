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
  double hipOffsetX;
  double footOffsetY;
  
  struct Stand {
    double speed;
  } stand;

  struct Walk {
    double comHeight;
    bool enableFSRProtection;
  } walk;
  
  IKParameters();
};

#endif // __IK__PARAMETERS_H_