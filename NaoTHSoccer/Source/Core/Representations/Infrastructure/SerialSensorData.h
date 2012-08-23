/* 
 * File:   SerialSensor.h
 * Author: claas
 *
 * Created on 18. August 2010, 16:58
 */

#ifndef _SERIALSENSORDATA_H
#define _SERIALSENSORDATA_H

#include "Tools/DataStructures/Streamable.h"
#include "PlatformInterface/PlatformInterchangeable.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/Math/Vector3.h"
#include <vector>

#define MAXSERIALREADTRYCOUNT 20
#define SERIALBUFFERLENGTH 1024
#define SERIALSENSOR_INIT_COUNT 100

using namespace naoth;

class SerialSensorData  : public Streamable, public PlatformInterchangeable, public Printable
{
  public:
    SerialSensorData();
    virtual ~SerialSensorData();

    virtual void toDataStream(ostream& os) const;
    virtual void fromDataStream(istream& is);
    virtual void print(ostream& stream) const;
    const vector<int>& getBoardIDs() const;
    double getMaxForce() const;
    double getForce(int id) const;
    Vector3<double> getAccVector(int id) const;
    Vector3<double> getAccVectorFiltered(int id) const;

    unsigned char rawData[SERIALBUFFERLENGTH];
    unsigned int rawDataLength;
    
    void processData();

  private:

    vector<vector<double> > accels;
    vector<vector<double> > forces;
    vector<int> ids;
};

class SerialSensorDataRequest : public Streamable, public PlatformInterchangeable
{
public:
  SerialSensorDataRequest() : receive(false) {}
  bool receive;

  virtual void toDataStream(ostream& os) const;
  virtual void fromDataStream(istream& is);
};


#endif  /* _SERIALSENSORDATA_H */

