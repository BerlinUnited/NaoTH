/* 
 * File:   LegacyConverter.h
 * Author: thomas
 *
 * Created on 24. September 2009, 13:25
 */

#ifndef _LEGACYCONVERTER_H
#define	_LEGACYCONVERTER_H

#include <iostream>

#include "Representations/Infrastructure/JointData.h"
#include "Representations/Infrastructure/Image.h"
#include "Messages/naothMessages.pb.h"

class LegacyConverter
{
public:

  static naothmessages::SensorJointData sensorJointDatafromStream(istream& stream, size_t size)
  {
    naothmessages::SensorJointData result;

    double position[JointData::numOfJoint];

    if ((JointData::numOfJoint * sizeof (double))*2 == size)
    {
      double hardness[JointData::numOfJoint];

      stream.read((char*) position, JointData::numOfJoint * sizeof (double));
      stream.read((char*) hardness, JointData::numOfJoint * sizeof (double));

      // only the newer one logged the hardness
      for (int i = 0; i < JointData::numOfJoint; i++)
      {
        result.mutable_jointdata()->add_hardness(hardness[i]);
      }
    } // check if this is an even older logfile using float
    else if ((JointData::numOfJoint * sizeof (float)) == size)
    {
      stream.read((char*) position, JointData::numOfJoint * sizeof (float));
    }

    // both have the position
    for (int i = 0; i < JointData::numOfJoint; i++)
    {
      result.mutable_jointdata()->add_position(position[i]);
    }

    return result;
  }//end sensorJointDatafromStream

  static void floatVectorFromStream(naothmessages::DoubleVector3* v, istream& stream)
  {
    char c;

    float x = 0.0;
    float y = 0.0;
    float z = 0.0;

    stream >> x;
    stream.read(&c, 1);
    if (c != ' ') cout << "Read Error x: " << c << endl;
    stream >> y;
    stream.read(&c, 1);
    if (c != ' ') cout << "Read Error y: " << c << endl;
    stream >> z;
    stream.read(&c, 1);
    if (c != ' ') cout << "Read Error z: " << c << endl;

    v->set_x(x);
    v->set_y(y);
    v->set_z(z);

  }//end vectorToStream

  static void doubleVectorFromStream(naothmessages::DoubleVector3* v, istream& stream)
  {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    
    stream.read((char*) (&x), 8);
    stream.read((char*) (&y), 8);
    stream.read((char*) (&z), 8);

    v->set_x(x);
    v->set_y(y);
    v->set_z(z);

  }

  static naothmessages::CameraMatrix cameraMatrixfromStream(istream& stream,  bool isFloat)
  {
    naothmessages::CameraMatrix result;

    for (int i = 0; i < 3; i++)
    {
      naothmessages::DoubleVector3* currentVec = result.mutable_pose()->add_rotation();
      if(isFloat)
      {
        floatVectorFromStream(currentVec, stream);
      }
      else
      {
        doubleVectorFromStream(currentVec, stream);
      }
    }//end for

    if(isFloat)
    {
      floatVectorFromStream(result.mutable_pose()->mutable_translation(), stream);
    }
    else
    {
      doubleVectorFromStream(result.mutable_pose()->mutable_translation(), stream);
    }

    return result;

  }//end fromDataStream


  static void oldRawImage(Image* image, istream& stream)
  {
    for(unsigned int i=0; i < image->getIndexSize(); i++)
    {
      unsigned int x = image->getXOffsetFromIndex(i);
      unsigned int y = image->getYOffsetFromIndex(i);
      
      char buf[3];

      stream.read(buf, 3);
      Pixel p;
      p.y = buf[0];
      p.u = buf[1];
      p.v = buf[2];

      image->set(x,y, p);
    }
  }

};

#endif	/* _LEGACYCONVERTER_H */

