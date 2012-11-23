/* 
 * File:   SerialSensor.cpp
 * Author: claas
 * 
 * Created on 18. August 2010, 16:58
 */

#include <vector>

#include "SerialSensorData.h"

#include "Messages/Representations.pb.h"
#include <google/protobuf/io/zero_copy_stream_impl.h>

#define FORCE_SENSOR_ZEROPOINT 5200

using namespace std;

SerialSensorData::SerialSensorData()
  : 
  rawDataLength(0)
{
  memset(&rawData[0], 0, SERIALBUFFERLENGTH * sizeof(unsigned char));
  accels.reserve(4);
  forces.reserve(4);
  ids.reserve(4);
}

SerialSensorData::~SerialSensorData()
{}

void SerialSensorData::processData()
{
  if(rawDataLength >= 24)
  {
    accels.clear();
    forces.clear();
    ids.clear();

    unsigned int boardCount = rawDataLength / 24;
    for (unsigned int i = 0; i < boardCount; i++)
    {
      // Format: 24 Bytes; 2 times 85 as 16bit header; 16bit ID; 16bit AccelX; 16bit AccelY; 16bit AccelZ; 6 Bytes free; 16bit force; 6 Bytes free
      if(rawData[i * 24] == 85 && rawData[i * 24 + 1] == 85)
      {
        int accelID = rawData[i * 24 + 2];
        ids.push_back(accelID);

        unsigned short accelX = rawData[i * 24 + 7];
        accelX = (accelX << 8) + rawData[i * 24 + 6];
        unsigned short accelY = rawData[i * 24 + 9];
        accelY = (accelY << 8) + rawData[i * 24 + 8];
        unsigned short accelZ = rawData[i * 24 + 11];
        accelZ = (accelZ << 8) + rawData[i * 24 + 10];

        vector<double> accel(3);
        accel[0] = (static_cast<short>(accelX) >> 6);
        accel[0] += 0.5; // disolve the assymetry of shorts (1bit more for negative values).
        accel[0] /= 512.0; // norm the maximum value to 1 and the minimum value to -1.
        accel[0] *= Math::g * 8.0; // the sensorscale reaches from -8g to 8g.
        accel[1] = (static_cast<short>(accelY) >> 6);
        accel[1] += 0.5;
        accel[1] /= 512.0;
        accel[1] *= - Math::g * 8.0;
        accel[2] = (static_cast<short>(accelZ) >> 6);
        accel[2] += 0.5;
        accel[2] /= 512.0;
        accel[2] *= Math::g * 8.0;

        // boards on the left arm are turned 180 degrees:
        if(accelID == 9 || accelID == 10 )
        {
           accel[0] *= -1.0;
        }
        accels.push_back(accel);

        unsigned short forceHand = rawData[i * 24 + 17];
        forceHand = (forceHand << 8) + rawData[i * 24 + 16];

        vector<double> force(2);

        force[0] = FORCE_SENSOR_ZEROPOINT;
        force[1] = (forceHand - force[0]) / 32768 * 15;
        forces.push_back(force);
      }
    }
  }//end for
}//end processData

void SerialSensorData::print(ostream& stream) const
{
  stream << "bord count : " << (rawDataLength / 24) << "\n";
  for(unsigned int i = 0; i < accels.size() && i < ids.size() && forces.size(); i++)
  {
    stream << "ID: " << ids[i] << "\tX: " << accels[i][0] << ",\tY: " << accels[i][1] << ",\tZ: " << accels[i][2] << "\tForces: (" << forces[i][1] <<  " N/" << forces[i][0] <<  " ZP)\n";
  }
  stream << "\nlength raw : " << rawDataLength << "\n";
  for(unsigned int i = 0; i < rawDataLength; i++)
  {
    if(i % 24 == 0)
    {
      stream << "\n ID " << (i / 24 + 7) << ": ";
    }
    stream << (int) rawData[i] << " ";
  }//end for
}//end print

void SerialSensorData::toDataStream(ostream& os) const 
{
  naothmessages::DoubleVector msg;

  // accels.size() == ids.size()
  for(unsigned int i = 0; i < accels.size() && i < ids.size() && forces.size(); i++)
  {
    msg.add_v(ids[i]);
    msg.add_v(accels[i][0]); // X
    msg.add_v(accels[i][1]); // Y
    msg.add_v(accels[i][2]); // Z
    msg.add_v(forces[i][0]); // Force Zero Point (16bit unsigned raw value)
    msg.add_v(forces[i][1]); // Force
  }//end for

  google::protobuf::io::OstreamOutputStream buf(&os);
  msg.SerializeToZeroCopyStream(&buf);
}//end toDataStream

void SerialSensorData::fromDataStream(istream& is) 
{
  naothmessages::DoubleVector msg;

  google::protobuf::io::IstreamInputStream buf(&is);
  msg.ParseFromZeroCopyStream(&buf);

  accels.clear();
  forces.clear();
  ids.clear();

  for(int i = 0; i < msg.v_size(); i+=4)
  {
    ids.push_back((int)msg.v(i));
    vector<double> accel(3);
    accel[0] = msg.v(i+1); // X
    accel[1] = msg.v(i+2); // Y
    accel[2] = msg.v(i+3); // Z
    accels.push_back(accel);
    vector<double> force(2);
    force[0] = msg.v(i+4); // Force Zero Point (16bit unsigned raw value)
    force[1] = msg.v(i+5); // Force
    forces.push_back(force);
  }//end for
}//end fromDataStream

const vector<int>& SerialSensorData::getBoardIDs() const {
  return ids;
}

/**
 *  Returns the maximum value of all force sensors.
 */
double SerialSensorData::getMaxForce() const {
  // TODO: also return negativ values if necessary.
  double maxForce = -1000.0;
  for (unsigned int i = 0; i < forces.size(); i++) {
    double curForce = forces[i][1];
  // TODO: determine sensor IDs dynamically.
    if(ids[i] == 8 || ids[i] == 10)
      maxForce = max(maxForce, curForce);
  }
  return maxForce;
}//end getMaxForce

/**
 *  Returns the force of the force-sensor with the given board ID,
 *  or -1000 if there is no sensor with the given board ID.
 */
double SerialSensorData::getForce(int id) const {
  // get the correct index for the force-array
  int index = -1;
  for(unsigned int i = 0; i < ids.size(); i++) {
    if (ids[i] == id) {
      index = i;
    }
  }

  // if an index was found, return the data, else return -1000
  if(index >= 0) {
    double result = forces[index][1];
    // apply offsets to the sensor values
    if (id == 8) {
      result += 1.238;
    } else if (id == 10) {
      result += -0.778;
    }
    return result;
  } else {
    return -1000.0;
  }
}

/**
 * Returns a vector<double> with the accelerator-sensor-values of the sensor with the given ID.
 * If there is no sensor with the given ID the vector (-1000, -1000, -1000) is returned.
 */
Vector3<double> SerialSensorData::getAccVector(int id) const {
  // get the correct index for the accels-array
  int index = -1;
  for(unsigned int i = 0; i < ids.size(); i++) {
    if (ids[i] == id) {
      index = i;
    }
  }

  // create a default vector
  Vector3<double> result;
  result.x = -1000.0;
  result.y = -1000.0;
  result.z = -1000.0;

  // if an index was found, return the data, else return the default vector
  if(index >= 0) {
    result.x = accels[index][0];
    result.y = accels[index][1];
    result.z = accels[index][2];
  }
  return result;
}



void SerialSensorDataRequest::toDataStream(ostream& os) const
{
  naothmessages::DoubleVector msg;
  msg.add_v(receive?1.0:0.0);
  google::protobuf::io::OstreamOutputStream buf(&os);
  msg.SerializeToZeroCopyStream(&buf);
}//end toDataStream

void SerialSensorDataRequest::fromDataStream(istream& is)
{
  naothmessages::DoubleVector msg;

  google::protobuf::io::IstreamInputStream buf(&is);
  msg.ParseFromZeroCopyStream(&buf);

  if(msg.v_size() == 1)
  {
    receive = msg.v(0) > 0;
  }//end for
}//end fromDataStream
