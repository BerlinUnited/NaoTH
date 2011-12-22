/**
* @file SupportPolygonGenerator.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, YUan</a>
* Declaration of the class SupportPolygonGenerator
*/

#ifndef _SUPPORTPOLYGONGENERATOR_H
#define  _SUPPORTPOLYGONGENERATOR_H

#include <Representations/Infrastructure/FSRData.h>
#include "Representations/Modeling/SupportPolygon.h"
#include "Representations/Modeling/KinematicChain.h"
#include "FootTouchDetector.h"

#include <DebugCommunication/DebugCommandExecutor.h>

class SupportPolygonGenerator: public DebugCommandExecutor
{
public:

  SupportPolygonGenerator();

  void init(const double* fsr, const Vector3<double>* fsrPos, const Kinematics::Link* link);

  ~SupportPolygonGenerator();

  void calcSupportPolygon(SupportPolygon& sp);

  Vector3<double> calcSupportForceCenter();

  /** test if the foot is supportable
   * @param isLeft indicates which foot is tested, true means left foot, false means right foot
   */
  bool isFootSupportable(bool isLeft) const;


  virtual void executeDebugCommand(
            const std::string& command, const std::map<std::string, std::string>& arguments,
            std::ostream &outstream);
  
private:

    const double* theFSRData;
    const Vector3<double>* theFSRPos;
    const Kinematics::Link* theLink;

    FootTouchDetector theLeftFootTouchDetector;
    FootTouchDetector theRightFootTouchDetector;
};


#endif  /* _SUPPORTPOLYGONGENERATOR_H */

