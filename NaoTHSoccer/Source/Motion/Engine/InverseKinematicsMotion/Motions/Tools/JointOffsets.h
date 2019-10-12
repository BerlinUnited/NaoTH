/**
* @file JointOffsets.h
*
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Steffen Kaden</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* 
*/

#ifndef _JointOffsets_H
#define _JointOffsets_H

#include <Representations/Infrastructure/JointData.h>

class JointOffsets
{
public:
    JointOffsets():minimalStep(0.0013962634){}

    void setMinimalStep(double minStep) {
        minimalStep = minStep;
    }

    void resetOffsets(){
        for(int i = 0; i < naoth::JointData::numOfJoint; i++){
            offsets.position[i] = 0;
        }
    }

    void increaseOffset(int i){
        offsets.position[i] += minimalStep;
    }

    void decreaseOffset(int i){
        offsets.position[i] -= minimalStep;
    }

    double operator [](int i){
        return offsets.position[i];
    }

private:
    JointData offsets;
    double minimalStep; // [rad]
};

#endif  /* _JointOffsets_H */

