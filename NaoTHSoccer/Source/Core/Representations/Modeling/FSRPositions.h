/**
* @file FSRPositions.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Definition of class FSRPositions
*/

#ifndef _FSRPositions_h_
#define _FSRPositions_h_

#include <Representations/Infrastructure/FSRData.h>

/**
* Represents positions of the FSR sensors based on the the KinematicChain
* TODO: put it together with the KinematicChain?
*/
class FSRPositions
{
public:
  Vector3<double> pos[naoth::FSRData::numOfFSR];
};

#endif // _FSRPositions_h_
