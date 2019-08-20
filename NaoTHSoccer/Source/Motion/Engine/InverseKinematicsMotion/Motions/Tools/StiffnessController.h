/**
* @file StiffnessController.h
*
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Steffen Kaden</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* 
*/

#ifndef _StiffnessController_H
#define _StiffnessController_H

#include <Tools/Math/Common.h>

class StiffnessController
{
public:
    StiffnessController():
        minAngle(0.08),
        maxAngle(2),
        minStiff(0.3),
        maxStiff(1.0)
    {}

    double control(double error) {
        // linear function
        double m = (maxStiff-minStiff)/(maxAngle-minAngle);
        double n = maxStiff-m*maxAngle;

        double e = fabs(error);

        if(e <= Math::fromDegrees(minAngle)){
            return minStiff;
        } else if(e > Math::fromDegrees(maxAngle)) {
            return maxStiff;
        } else {
            return m*Math::toDegrees(e) + n;
        }
    }

    void setMinMaxValues(double minAngle, double maxAngle, double minStiff, double maxStiff){
        this->minAngle = minAngle;
        this->maxAngle = maxAngle;
        this->minStiff = minStiff;
        this->maxStiff = maxStiff;
    }

private:
    double minAngle, maxAngle, minStiff, maxStiff;
};

#endif  /* _StiffnessController_H */

