/* 
 * File:   SerialSensor.h
 * Author: claas
 *
 * Created on 18. August 2010, 16:58
 */

#ifndef _SERIALSENSORDATA_H
#define _SERIALSENSORDATA_H

#include "Tools/DataStructures/Printable.h"
#include "Tools/Math/Vector3.h"
#include <vector>

#define MAXSERIALREADTRYCOUNT 20
#define SERIALBUFFERLENGTH 1024
#define SERIALSENSOR_INIT_COUNT 100

class SerialSensorData : public naoth::Printable
{
  public:
    SerialSensorData();
    virtual ~SerialSensorData();

    virtual void toDataStream(std::ostream& os) const;
    virtual void fromDataStream(std::istream& is);
    virtual void print(std::ostream& stream) const;
    const std::vector<int>& getBoardIDs() const;
    double getMaxForce() const;
    double getForce(int id) const;
    Vector3<double> getAccVector(int id) const;
    Vector3<double> getAccVectorFiltered(int id) const;

    unsigned char rawData[SERIALBUFFERLENGTH];
    unsigned int rawDataLength;
    
    void processData();

  private:

    std::vector<std::vector<double> > accels;
    std::vector<std::vector<double> > forces;
    std::vector<int> ids;
};

class SerialSensorDataRequest
{
public:
  SerialSensorDataRequest() : receive(false) {}
  bool receive;

  virtual void toDataStream(std::ostream& os) const;
  virtual void fromDataStream(std::istream& is);
};


#endif  /* _SERIALSENSORDATA_H */

