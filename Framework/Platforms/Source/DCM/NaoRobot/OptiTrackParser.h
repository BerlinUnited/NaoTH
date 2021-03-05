#ifndef OptiTrackParser_H
#define OptiTrackParser_H

#include <sstream>
#include <map>

#include <cmath>
#include <Tools/Math/Pose2D.h>
#include <Tools/Math/Pose3D.h>

#include <Tools/Communication/Network/BroadCaster.h>
#include <Tools/Debug/NaoTHAssert.h>

// maximally supported version
#define NETNAT_VERSION_0 3
#define NETNAT_VERSION_1 1
#define NETNAT_VERSION_2 0
#define NETNAT_VERSION_3 0

#define VERSION_NUMBER(v0, v1, v2, v3) (v3 + 100*(v2 + 100*(v1 + 100*v0)))
#define VALID_NETNAT_VERSION(v) \
  VERSION_NUMBER(v[0], v[1], v[2], v[3]) <= VERSION_NUMBER(NETNAT_VERSION_0, NETNAT_VERSION_1, NETNAT_VERSION_2, NETNAT_VERSION_3)

#include <Eigen/Core>
#include <Eigen/Geometry>
  
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
    // default version in our lab is 3.1.0.0
    // initialize to an invalide 
    natNetStreamVersion[0] = 0;
    natNetStreamVersion[1] = 0;
    natNetStreamVersion[2] = 0;
    natNetStreamVersion[3] = 0;
  }

  //~OptiTrackParser();

  RotationMatrix rotationFromQuaternion(float qx, float qy, float qz, float qw) const
  {
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

    return RotationMatrix(yaw, pitch, roll);
  }

  RotationMatrix rotationFromQuaternionDirectly(double w, double x, double y, double z) const {
    // taken from: https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation#Conversion_to_and_from_the_matrix_representation
    Vector3d c0(w*w + x*x - y*y - z*z, 2*x*y + 2*w*z,         2*x*z - 2*w*y);
    Vector3d c1(2*x*y - 2*w*z,         w*w - x*x + y*y - z*z, 2*y*z + 2*w*x);
    Vector3d c2(2*x*z + 2*w*y,         2*y*z - 2*w*x,         w*w - x*x - y*y + z*z);
    return RotationMatrix(c0, c1, c2);
  }


  std::string requestDefinitions() {
    std::stringstream ss;

    uint16_t cmd = NAT_REQUEST_MODELDEF;
    ss.write((const char*)&cmd, 2);

    uint16_t packetSize = 0;
    ss.write((const char*)&packetSize, 2);

    ss << '\0';
    return ss.str();
  }
  
  std::string requestPing() {
    std::stringstream ss;

    uint16_t cmd = NAT_PING;
    ss.write((const char*)&cmd, 2);

    uint16_t packetSize = 0;
    ss.write((const char*)&packetSize, 2);

    ss.write("Ping", 5);
    
    ss << '\0';
    return ss.str();
  }

  bool parseTrackable(std::stringstream& ss, Pose3D& pose, unsigned int& id)
  {
    ss.read((char*)&id, 4);

    Vector3f pos;
    ss.read((char*)&pos.x, 4);
    ss.read((char*)&pos.y, 4);
    ss.read((char*)&pos.z, 4);
    
    // rotation
    float qx, qy, qz, qw;
    ss.read((char*)&qx, 4);
    ss.read((char*)&qy, 4);
    ss.read((char*)&qz, 4);
    ss.read((char*)&qw, 4);

    // RB Marker Data ( Before version 3.0.  After Version 3.0 Marker data is in description )
    if(false) 
    {
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
    }
        
    float marker_error = 0.0f;
    ss.read((char*)&marker_error, 4);
    //std::cout << "error: " << marker_error << std::endl;
        
    //Version 2.6 and later
    unsigned short trackingValid = 0;
    ss.read((char*)&trackingValid, 2);
    
    if (!trackingValid) {
      return false;
    }
        
    // accept only valid transformations
    if( !std::isfinite(pos.x) || !std::isfinite(pos.y) || !std::isfinite(pos.z) ||
        !std::isfinite(qx) || !std::isfinite(qy) || !std::isfinite(qz) || !std::isfinite(qw) ) 
    {
      std::cout << "[OptiTrackParser] invalide value" << std::endl;
      return false;
    }

    // this trackable is not tracked right now
    if (qx == 0) {
      return false;
    }

    // OptiTrack coordinate system is using the NUE convention, i.e.,
    // X-North, Y-Up, Z-East.
    // In the NaoTH Lab the OptiTrack System is calibrated such, that
    //   Z - pointing towards opponent goal
    //   X - pointing left when facing opponent goal,
    //   Y - is pointing up
    // Convert from the NaoTH-Optitrac Setup to Global RoboCup Coordinate System.
    pose.translation.x = pos.z;
    pose.translation.y = pos.x;
    pose.translation.z = pos.y;

    // converto to mm
    pose.translation *= 1000.0;

    // calculate the rotation
    // NOTE: convert to Global RoboCup Coordinate System as described above.
    pose.rotation = rotationFromQuaternionDirectly(qw, qz, qx, qy);

    /*
    // the following code was used to test the conversion function rotationFromQuaternionDirectly
    Eigen::Matrix3d R;
    R << pose.rotation[0][0], pose.rotation[1][0], pose.rotation[2][0],
         pose.rotation[0][1], pose.rotation[1][1], pose.rotation[2][1],
         pose.rotation[0][2], pose.rotation[1][2], pose.rotation[2][2];
    
    // test for correctness
    Eigen::Quaterniond q = Eigen::Quaterniond(R).normalized();
    
    std::cout << R.determinant() << std::endl;
    // compare
    std::cout << qw << "\t" << qz << "\t" << qx << "\t" << qy << std::endl;
    std::cout << -q.w() << "\t" << -q.x() << "\t" << -q.y() << "\t" << -q.z() << std::endl << std::endl;
    */
    
    return true;
  }// end parseTrackable

  
  std::string parseMessage(const std::string& msg) 
  {  
    //trackablesTimeStamp = NaoTime::getNaoTimeInMilliSeconds();
    std::stringstream ss(msg);

    unsigned short messageID(0);
    ss.read((char*)&messageID, 2);

    unsigned short packetSize(0);
    ss.read((char*)&packetSize, 2);

    // NetNat Version is not valid, wait until we geta valid version
    if(natNetStreamVersion[0] == 0 && messageID != NAT_PINGRESPONSE) {
      return requestPing();
    }
    
    if(messageID == NAT_PINGRESPONSE)
    {
      // Skip the sending app's Name field
      ss.seekg(256, std::ios_base::cur);
      // Skip the sending app's Version info
      ss.seekg(4, std::ios_base::cur);

      ss.read((char*)&natNetStreamVersion, 4);

      std::cout << "[OptiTrackParser] NatNetStreamVersion (" 
                << (int)natNetStreamVersion[0] << "," 
                << (int)natNetStreamVersion[1] << "," 
                << (int)natNetStreamVersion[2] << "," 
                << (int)natNetStreamVersion[3] << ")" << std::endl;
                
      // make sure the current parser supports the NetNat Version
      ASSERT(VALID_NETNAT_VERSION(natNetStreamVersion));
    }
    else if(messageID == NAT_MODELDEF) 
    {
      std::cout << "[OptiTrackParser] update definitions: " << std::endl;
      trackable_names.clear();
      
      unsigned int datasetCount = 0;
      ss.read((char*)&datasetCount, 4);
      
      for(unsigned int i = 0; i < datasetCount; ++i) 
      {
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
          
          //std::cout << "[OptiTrackParser] trackable: " << "(" << id << ") " << name << std::endl;
          
          // Version 3.0 and higher, rigid body marker information contained in description
          if(true)
          {
            unsigned int markerCount = 0;
            ss.read((char*)&markerCount, 4);
            
            // marker markerOffset
            for (unsigned int j = 0; j < markerCount; ++j) {
              Vector3f m;
              ss.read((char*)&m.x, 4);
              ss.read((char*)&m.y, 4);
              ss.read((char*)&m.z, 4);
              // TODO: do nothing for now
            }
                
            // marker activeLabel
            for (unsigned int j = 0; j < markerCount; ++j) {
              unsigned int m_id = 0;
              ss.read((char*)&m_id, 4);
              // TODO: do nothing for now
            }
          }
          
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

      for (unsigned int i = 0; i < rigidBodyCount; ++i) 
      {
        unsigned int id = 0;
        Pose3D pose;
        if(!parseTrackable(ss, pose, id)) {
          continue;
        }
        
        if(!addTrackable(id, pose)) {
          reset();
          return requestDefinitions();
        }
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
      std::cout << "[OptiTrackParser] Unknown trackable id: " << id << std::endl;
      return false;
    }

    trackables.insert(std::make_pair(name_it->second, pose));
    return true;
  }

  std::map<std::string,Pose3D> trackables;
  std::map<unsigned int,std::string> trackable_names;
  //unsigned int trackablesTimeStamp;

  uint8_t natNetStreamVersion[4];
};

#endif // OptiTrackParser_H
