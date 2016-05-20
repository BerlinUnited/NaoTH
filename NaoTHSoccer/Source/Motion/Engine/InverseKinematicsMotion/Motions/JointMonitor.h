#ifndef JOINTMONITOR_H
#define JOINTMONITOR_H

#include <Tools/DataStructures/RingBufferWithSum.h>
#include <Tools/DataStructures/RingBuffer.h>

//template <int bufferSizeError, int bufferSizeCurrent>
class JointMonitor
{
public:
    JointMonitor(){
    }

    void updateMonitor(double motorData, double sensorData, double cumCurrent) {
        motorJointDataBuffer.add(motorData);

        if(motorJointDataBuffer.isFull()){
            motorToSensorError.add(fabs(sensorData - motorJointDataBuffer.first()));
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

    double getOldestMotorData(){
        return motorJointDataBuffer.first();
    }

    bool motorJointDataBufferIsFull(){
        return motorJointDataBuffer.isFull();
    }

private:
    RingBuffer<double,4> motorJointDataBuffer;
    RingBufferWithSum<double,1>   motorToSensorError;
    RingBufferWithSum<double,1> current;
};

#endif // JOINTMONITOR_H
