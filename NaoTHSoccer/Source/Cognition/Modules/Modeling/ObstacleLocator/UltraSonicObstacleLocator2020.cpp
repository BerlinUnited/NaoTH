#include "UltraSonicObstacleLocator2020.h"

UltraSonicObstacleLocator2020::UltraSonicObstacleLocator2020()
{
    //DEBUG_REQUEST_REGISTER("USOL2020:draw3D:measurements", "", false);

    rightReceiverInTorso.translation = {47.7, -41.6, 50.9};
    leftReceiverInTorso.translation  = {47.7,  41.6, 50.9};

    rightReceiverInTorso.rotation = RotationMatrix::getRotationZ(-0.4363)
                                    * RotationMatrix::getRotationY(0.2618);
    leftReceiverInTorso.rotation = RotationMatrix::getRotationZ(0.4363)
                               * RotationMatrix::getRotationY(0.2618);
}

void UltraSonicObstacleLocator2020::execute()
{
    Pose3D torso = getKinematicChain().theLinks[KinematicChain::Torso].M;
    for(int i = 0; i < UltraSoundReceiveData::numOfUSEcho; i++) {
        leftInWorld[i] = torso * leftReceiverInTorso * Vector3d(getUltraSoundReceiveData().dataLeft[i] * 1000,0,0);
    }
    for(int i = 0; i < UltraSoundReceiveData::numOfUSEcho; i++) {
        rightInWorld[i] = torso * rightReceiverInTorso * Vector3d(getUltraSoundReceiveData().dataRight[i] * 1000,0,0);
    }
    // 2. filter ground (-> later assume reflection?)
    // 3. provide obstacle model measurements

    draw();
}

//bool UltraSonicObstacleLocator2020::is_new_data_avaliable() const
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

void UltraSonicObstacleLocator2020::draw() const
{
    //DEBUG_REQUEST("USOL2020:draw3D:measurements",
        for(int i = 0; i < UltraSoundReceiveData::numOfUSEcho; i++) {
            SPHERE("0000FF", 10, leftInWorld[i]);
        }
        for(int i = 0; i < UltraSoundReceiveData::numOfUSEcho; i++) {
            SPHERE("FF0000", 10, rightInWorld[i]);
        }
    //);
}
