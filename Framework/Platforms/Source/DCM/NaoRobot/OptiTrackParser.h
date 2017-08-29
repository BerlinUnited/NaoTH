#ifndef OptiTrackParser_H
#define OptiTrackParser_H

#include <sstream>
#include <map>

#include <Tools/Math/Pose2D.h>
#include <Tools/Math/Pose3D.h>

#include <Tools/Communication/Network/BroadCaster.h>

class OptiTrackParser
{
public:

  static const int NAT_PING                  = 0;
  static const int NAT_PINGRESPONSE          = 1;
  static const int NAT_REQUEST               = 2;
  static const int NAT_RESPONSE              = 3;
  static const int NAT_REQUEST_MODELDEF      = 4;
  static const int NAT_MODELDEF              = 5;
  static const int NAT_REQUEST_FRAMEOFDATA   = 6;
  static const int NAT_FRAMEOFDATA           = 7;
  static const int NAT_MESSAGESTRING         = 8;
  static const int NAT_DISCONNECT            = 9;
  static const int NAT_UNRECOGNIZED_REQUEST  = 100;
  
public:
  OptiTrackParser()
  {
    requestDefinitions();
  }
  //~OptiTrackParser();

  std::string requestDefinitions() {
    std::stringstream ss;

    uint16_t cmd = NAT_REQUEST_MODELDEF;
    ss.write((const char*)&cmd, 2);

    uint16_t packetSize = 0;
    ss.write((const char*)&packetSize, 2);

    ss << '\0';
    return ss.str();
  }
  
  std::string parseMessage(const std::string& msg) 
  {  
    std::stringstream ss(msg);

    unsigned short messageID(0);
    ss.read((char*)&messageID, 2);

    unsigned short packetSize(0);
    ss.read((char*)&packetSize, 2);

    if(messageID == NAT_MODELDEF) 
    {
      std::cout << "[OptiTrackParser] update definitions: " << std::endl;
      trackable_names.clear();
      
      unsigned int datasetCount = 0;
      ss.read((char*)&datasetCount, 4);
      
      for(unsigned int i = 0; i < datasetCount; ++i) {
        unsigned int type = 0;
        ss.read((char*)&type, 4);
        
        if(type == 0) // __unpackMarkerSetDescription
        {
          std::string name;
          std::getline( ss, name, '\0' );
          
          unsigned int markerCount = 0;
          ss.read((char*)&markerCount, 4);
          
          for(unsigned int j = 0; j < markerCount; ++j) {
            std::string marker_name;
            std::getline( ss, marker_name, '\0' );
          }
          
        } 
        else if(type == 1) 
        {
          std::string name;
          std::getline( ss, name, '\0' );
          
          unsigned int id = 0;
          ss.read((char*)&id, 4);
          
          unsigned int parentID = 0;
          ss.read((char*)&parentID, 4);
          
          // skip the timestamp
          ss.seekg(12, std::ios_base::cur);
          
          std::cout << "[OptiTrackParser] trackable: " << "(" << id << ") " << name << std::endl;
          
          trackable_names.insert(std::make_pair(id, name));
        } 
        else if(type == 2) // __unpackSkeletonDescription
        {
          std::cout << "[OptiTrackParser] unsupported description type: " << type << std::endl;
          assert(false);
        }
        else 
        {
          std::cout << "[OptiTrackParser] unsupported description type: " << type << std::endl;
          assert(false);
        }
      }
      
    }
    else if (messageID == NAT_FRAMEOFDATA) 
    {
      unsigned int frameNumber = 0;
      ss.read((char*)&frameNumber, 4);

      unsigned int markerSetCount = 0;
      ss.read((char*)&markerSetCount, 4);
      
      //ASSERT(markerSetCount == 0); // not supported

      for (unsigned int i = 0; i < markerSetCount; ++i)
      {
        // read the name of the representation
        std::string modelName;
        std::getline( ss, modelName, '\0' );
        
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

        //std::cout << "id: " << id << std::endl;


        // rotation
        float qx, qy, qz, qw;
        ss.read((char*)&qx, 4);
        ss.read((char*)&qy, 4);
        ss.read((char*)&qz, 4);
        ss.read((char*)&qw, 4);

        
        unsigned int markerCount = 0;
        ss.read((char*)&markerCount, 4);
        
        // marker positions
        for (unsigned int j = 0; j < markerCount; ++j) {
          Vector3f m;
          ss.read((char*)&m.x, 4);
          ss.read((char*)&m.y, 4);
          ss.read((char*)&m.z, 4);
          // TODO: do nothing for now
        }
        
        // marker id's
        for (unsigned int j = 0; j < markerCount; ++j) {
          unsigned int m_id = 0;
          ss.read((char*)&m_id, 4);
          // TODO: do nothing for now
        }
        
        // marker sizes
        for (unsigned int j = 0; j < markerCount; ++j) {
          float m_size = 0.0f;
          ss.read((char*)&m_size, 4);
          // TODO: do nothing for now
        }
        
        float marker_error = 0.0f;
        ss.read((char*)&marker_error, 4);
        //std::cout << "error: " << marker_error << std::endl;
        
        //Version 2.6 and later
        //unsigned short trackingValid = 0;
        //ss.read((char*)&trackingValid, 2);
        
        
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
        if(!addTrackable(id, pose3)) {
          reset();
          return requestDefinitions();
        }
        
        //yaw = pose3.rotation.getZAngle();
        //pitch = pose3.rotation.getYAngle();
        //roll = pose3.rotation.getXAngle();

        //Pose2D pose(-data.data.rotation.getYAngle() + Math::fromDegrees(27.0), data.data.translation.x, data.data.translation.y);
        //pose.translate(150.0, 0.0);
        //data.data = Pose3D::embedXY(pose);
      }
    }
    else {
      std::cout << "[OptiTrackParser] Unknown message: " << messageID << std::endl;
      assert(false);
    }
    
    return std::string();
  }
  
  void reset() {
    trackables.clear();
  }
  
  const std::map<std::string,Pose3D>& getTrackables() const {
    return trackables;
  }
  
private:
  bool addTrackable(unsigned int id, const Pose3D& pose) {
    std::map<unsigned int,std::string>::iterator name_it = trackable_names.find(id);
    if(name_it == trackable_names.end()) {
      std::cout << "[OptiTrackParser] Unknown id: " << id << std::endl;
      return false;
    }
    
    trackables.insert(std::make_pair(name_it->second, pose));
    return true;
  }

  std::map<std::string,Pose3D> trackables;
  std::map<unsigned int,std::string> trackable_names;

};

#endif // OptiTrackParser_H
