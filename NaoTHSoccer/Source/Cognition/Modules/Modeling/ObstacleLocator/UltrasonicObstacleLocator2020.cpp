#include "UltrasonicObstacleLocator2020.h"

UltrasonicObstacleLocator2020::UltrasonicObstacleLocator2020()
{
    DEBUG_REQUEST_REGISTER("UltrasonicObstacleLocation2020:draw3D:measurements", "", false);

    rightReceiverInTorso.translation = {47.7, -41.6, 50.9};
    leftReceiverInTorso.translation  = {47.7,  41.6, 50.9};

    rightReceiverInTorso.rotation = RotationMatrix::getRotationZ(-0.4363)
                                    * RotationMatrix::getRotationY(0.2618);
    leftReceiverInTorso.rotation = RotationMatrix::getRotationZ(0.4363)
                                   * RotationMatrix::getRotationY(0.2618);

    rightInWorld.reserve(UltraSoundReceiveData::numOfUSEcho);
    leftInWorld.reserve(UltraSoundReceiveData::numOfUSEcho);

    getDebugParameterList().add(&parameter);
}

void UltrasonicObstacleLocator2020::execute()
{
    // reset percept representation and measuremnt vectors
    getUltrasonicObstaclePercept().location_on_ground.clear();
    leftInWorld.clear();
    rightInWorld.clear();

    // transform measurements into world coordinates if not too far away
    Pose3D torso = getKinematicChain().theLinks[KinematicChain::Torso].M;
    for(int i = 0; i < UltraSoundReceiveData::numOfUSEcho; i++) {
        if(getUltraSoundReceiveData().dataLeft[i] < 5.0) { // == 5.0 means no echo
            leftInWorld.push_back(torso * leftReceiverInTorso  * Vector3d(getUltraSoundReceiveData().dataLeft[i] * 1000,0,0));
        }
        if(getUltraSoundReceiveData().dataRight[i] < 5.0) { // == 5.0 means no echo
            rightInWorld.push_back(torso * rightReceiverInTorso  * Vector3d(getUltraSoundReceiveData().dataRight[i] * 1000,0,0));
        }
    }

    // filter out ground (-> later assume reflection?)
    for(auto& point : leftInWorld){
        if(point.z > parameter.ground_threshold){
            getUltrasonicObstaclePercept().location_on_ground.push_back({point.x, point.y});
        }
    }

    for(auto& point : rightInWorld){
        if(point.z > parameter.ground_threshold){
            getUltrasonicObstaclePercept().location_on_ground.push_back({point.x, point.y});
        }
    }

    // 3. provide obstacle model measurements

    draw();
}

//bool UltrasonicObstacleLocator2020::is_new_data_avaliable() const
//{
//  for(int i = 0; i < UltraSoundReceiveData::numOfUSEcho; i++) {
//    if(getUltraSoundReceiveData().dataLeft[i] != lastValidUltraSoundReceiveData.dataLeft[i] ||
//       getUltraSoundReceiveData().dataRight[i] != lastValidUltraSoundReceiveData.dataRight[i])
//    {
//      return true;
//    }
//  }
//  return false;
//}

void UltrasonicObstacleLocator2020::draw() const
{
    DEBUG_REQUEST("UltrasonicObstacleLocation2020:draw3D:measurements",
        for(auto& point : leftInWorld){
            SPHERE("0000FF", 10, point);
        }
        for(auto& point : rightInWorld){
            SPHERE("FF0000", 10, point);
        }
    );
}
