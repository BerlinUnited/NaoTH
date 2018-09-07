/**
* @file CoPProvider.h
*
* @author <a href="mailto:kaden@informatik.hu-berlin.de">Kaden, Steffen</a>
*/

#ifndef _COP_PROVIDER_H
#define _COP_PROVIDER_H

#include <ModuleFramework/Module.h>

#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Infrastructure/FSRData.h"
#include "Representations/Modeling/CentreOfPressure.h"

#include "Tools/NaoInfo.h"

BEGIN_DECLARE_MODULE(CoPProvider)
  REQUIRE(FSRData)
  REQUIRE(KinematicChainSensor)

  PROVIDE(CentreOfPressure)
END_DECLARE_MODULE(CoPProvider)

class CoPProvider : private CoPProviderBase
{
public:
  CoPProvider(){}

  virtual void execute(){
    const double epsilon = 1e-6;

    Vector3d left_CoP, right_CoP;
    double total_Pressure_left  = 0;
    double total_Pressure_right = 0;
    for(int i = 0; i < FSRData::numOfFSR; i++){
      left_CoP  += NaoInfo::FSRPositionsLeft[i]  * getFSRData().dataLeft[i];
      right_CoP += NaoInfo::FSRPositionsRight[i] * getFSRData().dataRight[i];

      total_Pressure_left  += getFSRData().dataLeft[i];
      total_Pressure_right += getFSRData().dataRight[i];
    }

    if(fabs(total_Pressure_left) < epsilon) {
      getCentreOfPressure().in_and_only_left_foot.CoP = Vector3d();
      getCentreOfPressure().in_and_only_left_foot.magnitude = 0;
      getCentreOfPressure().in_and_only_left_foot.valid = false;
    } else {
      getCentreOfPressure().in_and_only_left_foot.CoP = left_CoP / total_Pressure_left;
      getCentreOfPressure().in_and_only_left_foot.magnitude = total_Pressure_left;
      getCentreOfPressure().in_and_only_left_foot.valid = true;
    }

    if(fabs(total_Pressure_right) < epsilon) {
      getCentreOfPressure().in_and_only_right_foot.CoP = Vector3d();
      getCentreOfPressure().in_and_only_right_foot.magnitude = 0;
      getCentreOfPressure().in_and_only_right_foot.valid = false;
    } else {
      getCentreOfPressure().in_and_only_right_foot.CoP = right_CoP / total_Pressure_right;
      getCentreOfPressure().in_and_only_right_foot.magnitude = total_Pressure_right;
      getCentreOfPressure().in_and_only_right_foot.valid = true;
    }

    if(getCentreOfPressure().in_and_only_left_foot.valid || getCentreOfPressure().in_and_only_right_foot.valid) {
        getCentreOfPressure().in_kinematic_chain_origin.valid = true;

        // check: following might be only true if standing on flat ground
        getCentreOfPressure().in_kinematic_chain_origin.magnitude = getCentreOfPressure().in_and_only_left_foot.magnitude + getCentreOfPressure().in_and_only_right_foot.magnitude;

        Vector3d left_CoP_in_origin  = getKinematicChainSensor().theLinks[KinematicChain::LFoot].M * getCentreOfPressure().in_and_only_left_foot.CoP;
        Vector3d right_CoP_in_origin = getKinematicChainSensor().theLinks[KinematicChain::RFoot].M * getCentreOfPressure().in_and_only_right_foot.CoP;

        getCentreOfPressure().in_kinematic_chain_origin.CoP = (  left_CoP_in_origin  * getCentreOfPressure().in_and_only_left_foot.magnitude
                                                               + right_CoP_in_origin * getCentreOfPressure().in_and_only_right_foot.magnitude )
                                                              / getCentreOfPressure().in_kinematic_chain_origin.magnitude;
    } else {
        getCentreOfPressure().in_kinematic_chain_origin.CoP = Vector3d();
        getCentreOfPressure().in_kinematic_chain_origin.magnitude = 0;
        getCentreOfPressure().in_kinematic_chain_origin.valid = false;
    }
  }
};

#endif // _COP_PROVIDER_H
