/**
* @file SupportPolygonGenerator.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, YUan</a>
* Declaration of the class SupportPolygonGenerator
*/

#ifndef _SupportPolygonGenerator_h_
#define _SupportPolygonGenerator_h_

#include <ModuleFramework/Module.h>

// representations
#include <Representations/Infrastructure/FSRData.h>
#include "Representations/Modeling/SupportPolygon.h"
#include "Representations/Modeling/KinematicChain.h"
#include "Representations/Modeling/FSRPositions.h"

// tools
#include "FootTouchDetector.h"

// debug
#include <DebugCommunication/DebugCommandExecutor.h>


BEGIN_DECLARE_MODULE(SupportPolygonGenerator)
  REQUIRE(FSRData)
  REQUIRE(FSRPositions)
  REQUIRE(KinematicChainSensor)

  PROVIDE(SupportPolygon)
END_DECLARE_MODULE(SupportPolygonGenerator)


class SupportPolygonGenerator: public SupportPolygonGeneratorBase, public DebugCommandExecutor
{
public:

  SupportPolygonGenerator();
  ~SupportPolygonGenerator();

  void execute();

  Vector3<double> calcSupportForceCenter();

  /** test if the foot is supportable
   * @param isLeft indicates which foot is tested, true means left foot, false means right foot
   */
  bool isFootSupportable(bool isLeft) const;


  virtual void executeDebugCommand(
            const std::string& command, const std::map<std::string, std::string>& arguments,
            std::ostream &outstream);
  
private:

  //TODO: remove it
  const double* theFSRData;
  const Vector3<double>* theFSRPos;
  const Kinematics::Link* theLink;

  FootTouchDetector theLeftFootTouchDetector;
  FootTouchDetector theRightFootTouchDetector;
};


#endif  /* _SupportPolygonGenerator_h_ */

