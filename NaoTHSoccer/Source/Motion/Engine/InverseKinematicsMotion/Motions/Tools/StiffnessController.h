/**
* @file StiffnessController.h
*
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Steffen Kaden</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* 
*/

#ifndef STIFFNESS_CONTROLLER_H
#define STIFFNESS_CONTROLLER_H

#include <Tools/Math/Common.h>

class StiffnessController
{
public:
    StiffnessController():
        minAngle(0.08),
        maxAngle(2),
        minStiff(0.3),
        maxStiff(1.0)
    {
      setMinMaxValues(minAngle, maxAngle, minStiff, maxStiff);
    }


    double control(double error) const
    {
        double e = fabs(error);

        if(e <= minAngle) {
            return minStiff;
        } else if(e > maxAngle) {
            return maxStiff;
        } else {
            return m*e + n;
        }
    }

    /**
    * maxStiff - [0,1] maximal stiffness to be applied
    * minStiff - [0,1] minimal stiffness to be applied
    * minAngle - [rad] angle below which minStiff is applied
    * maxAngle - [rad] angle above which maxStiff is applied
    */
    void setMinMaxValues(double minAngle, double maxAngle, double minStiff, double maxStiff) {
        this->minAngle = minAngle;
        this->maxAngle = maxAngle;
        this->minStiff = minStiff;
        this->maxStiff = maxStiff;

        // Compute parameters for a linear function f(x) = m*x + n, with
        //   f(minAngle) == minStiff
        //   f(maxAngle) == maxStiff
        m = (maxStiff-minStiff) / (maxAngle-minAngle);
        n = maxStiff-m*maxAngle;
    }

private:
    double minAngle, maxAngle, minStiff, maxStiff;
    
    // parameter for the linear function
    // f(x) = m*x + n
    double m, n;
};

#endif  /* STIFFNESS_CONTROLLER_H */

