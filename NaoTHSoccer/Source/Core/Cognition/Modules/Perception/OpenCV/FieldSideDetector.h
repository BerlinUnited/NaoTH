#ifndef FIELDSIDEDETECTOR_H
#define FIELDSIDEDETECTOR_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/Image.h>
#include "Representations/Perception/CameraMatrix.h"
#include <Representations/Perception/GoalPercept.h>
#include <DebugCommunication/DebugCommandExecutor.h>

#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>

BEGIN_DECLARE_MODULE(FieldSideDetector)
  REQUIRE(GoalPercept)
  REQUIRE(Image)
  REQUIRE(CameraMatrix)
END_DECLARE_MODULE(FieldSideDetector)

class FieldSideDetector : public FieldSideDetectorBase, DebugCommandExecutor
{
  public:
  FieldSideDetector();

  virtual void execute();
  virtual void executeDebugCommand(const std::string &command,
                                   const std::map<std::string, std::string> &arguments,
                                   std::ostream &outstream);

  virtual ~FieldSideDetector();
  private:
    cv::Mat trainInput;
    cv::Mat trainOutput;

    CvDTree dtree;
};

#endif // FIELDSIDEDETECTOR_H
