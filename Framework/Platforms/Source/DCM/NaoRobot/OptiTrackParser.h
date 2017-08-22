#ifndef OptiTrackParser_H
#define OptiTrackParser_H

#include <sstream>
#include <map>

#include <Tools/Math/Pose2D.h>
#include <Tools/Math/Pose3D.h>

class OptiTrackParser
{
public:
  static const int NAT_FRAMEOFDATA = 7;
  
  
public:
  //OptiTrackParser();
  //~OptiTrackParser();

  
  void parseMessage(const std::string& msg) {
    
    stringstream ss(msg);

    unsigned short messageID(0);
    ss.read((char*)&messageID, 2);

    unsigned short packetSize(0);
    ss.read((char*)&packetSize, 2);

    if (messageID == NAT_FRAMEOFDATA) {
      unsigned int frameNumber = 0;
      ss.read((char*)&frameNumber, 4);

      unsigned int markerSetCount = 0;
      ss.read((char*)&markerSetCount, 4);
      
      //ASSERT(markerSetCount == 0); // not supported

      for (unsigned int i = 0; i < markerSetCount; ++i)
      {
        // read the name of the representation
        std::string modelName;
        char c = '\0';
        ss.read(&c, 1);
        while (c != '\0') {
          modelName += c;
          ss.read(&c, 1);
        }
        
        unsigned int markerCount = 0;
        ss.read((char*)&markerCount, 4);

        for (unsigned int i = 0; i < markerCount; ++i) {
          Vector3f pos;
          ss.read((char*)&pos.x, 4);
          ss.read((char*)&pos.y, 4);
          ss.read((char*)&pos.z, 4);
        }
      }

      unsigned int unlabeledMarkersCount = 0;
      ss.read((char*)&unlabeledMarkersCount, 4);
      
      for (unsigned int i = 0; i < unlabeledMarkersCount; ++i) {
        Vector3f pos;
        ss.read((char*)&pos.x, 4);
        ss.read((char*)&pos.y, 4);
        ss.read((char*)&pos.z, 4);
      }

      unsigned int rigidBodyCount = 0;
      ss.read((char*)&rigidBodyCount, 4);

      for (unsigned int i = 0; i < rigidBodyCount; ++i) {
        unsigned int id = 0;
        ss.read((char*)&id, 4);

        Vector3f pos;
        ss.read((char*)&pos.x, 4);
        ss.read((char*)&pos.y, 4);
        ss.read((char*)&pos.z, 4);

        Vector3f new_pos;
        new_pos.x = -pos.z;
        new_pos.y = -pos.x;
        new_pos.z =  pos.y;

        Pose3D pose3;
        pose3.translation.x = new_pos.x;
        pose3.translation.y = new_pos.y;
        pose3.translation.z = new_pos.z;
        pose3.translation *= 1000.0;

        //std::cout << data.data.translation.x << " " << data.data.translation.y << " " << data.data.translation.z << id << std::endl;


        // rotation
        float qx, qy, qz, qw;
        ss.read((char*)&qx, 4);
        ss.read((char*)&qy, 4);
        ss.read((char*)&qz, 4);
        ss.read((char*)&qw, 4);

        if (qx == 0) {
          continue;
        }

        // convert to Pose2d
        double ysqr = qy * qy;

        // roll(x - axis rotation)
        double t0 = +2.0 * (qw * qx + qy * qz);
        double t1 = +1.0 - 2.0 * (qx * qx + ysqr);
        double roll = atan2(t0, t1);

        //pitch(y - axis rotation)
        double t2 = +2.0 * (qw * qy - qz * qx);
        t2 = t2 > 1.0f ? 1.0 : t2;
        t2 = t2 < -1.0f ? -1.0 : t2;
        double pitch = asin(t2); // HACK: constant offset

        //yaw(z - axis rotation)
        double t3 = +2.0 * (qw * qz + qx * qy);
        double t4 = +1.0 - 2.0 * (ysqr + qz * qz);
        double yaw = atan2(t3, t4);

        pose3.rotation = RotationMatrix(yaw, pitch, roll);
        trackables.insert(std::make_pair(id, pose3));
        
        //yaw = pose3.rotation.getZAngle();
        //pitch = pose3.rotation.getYAngle();
        //roll = pose3.rotation.getXAngle();

        //Pose2D pose(-data.data.rotation.getYAngle() + Math::fromDegrees(27.0), data.data.translation.x, data.data.translation.y);
        //pose.translate(150.0, 0.0);
        //data.data = Pose3D::embedXY(pose);
      }
    }
  }
  
  void reset() {
    trackables.clear();
  }
  
  const std::map<unsigned int,Pose3D>& getTrackables() const {
    return trackables;
  }
  
private:
  std::map<unsigned int,Pose3D> trackables;
};

#endif // OptiTrackParser_H
