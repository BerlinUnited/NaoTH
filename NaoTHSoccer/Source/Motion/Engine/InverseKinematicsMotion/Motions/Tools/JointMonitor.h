/**
* @file JointMonitor.h
*
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Steffen Kaden</a>
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* 
*/

#ifndef _JointMonitor_H
#define _JointMonitor_H

#include <Tools/DataStructures/RingBufferWithSum.h>

class JointMonitor
{
public:
    JointMonitor() {}

    void updateMonitor(double motorData, double sensorData, double cumCurrent) {
        motorJointDataBuffer.add(motorData);

        if(motorJointDataBuffer.isFull()){
            motorToSensorError.add(sensorData - motorJointDataBuffer.first());
        }

        current.add(cumCurrent);
    }

    void resetAll() {
        motorJointDataBuffer.clear();
        motorToSensorError.clear();
        current.clear();
    }

    void resetError() {
        motorToSensorError.clear();
    }

    double getError(){
        return motorToSensorError.getAverage();
    }

    double filteredCurrent(){
        return current.getAverage();
    }

private:
    RingBuffer<double,4> motorJointDataBuffer;
    RingBufferWithSum<double,100> motorToSensorError;
    RingBufferWithSum<double,200> current;
};

#endif  /* _JointMonitor_H */

