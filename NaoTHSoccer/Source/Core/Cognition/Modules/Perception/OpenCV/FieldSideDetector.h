#ifndef FIELDSIDEDETECTOR_H
#define FIELDSIDEDETECTOR_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/Image.h>
#include <Representations/Perception/ArtificialHorizon.h>
#include "Representations/Perception/CameraMatrix.h"
#include <Representations/Perception/GoalPercept.h>
#include <DebugCommunication/DebugCommandExecutor.h>
#include <Representations/Perception/FieldSidePercept.h>

#if defined(__GNUC__) && defined(_NAOTH_CHECK_CONVERSION_)
#if __GNUC__ > 3 && __GNUC_MINOR__ > 5
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic ignored "-Wconversion"
#endif
#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>
#if defined(__GNUC__) && defined(_NAOTH_CHECK_CONVERSION_)
#if __GNUC__ > 3 && __GNUC_MINOR__ > 5
#pragma GCC diagnostic push
#endif
#pragma GCC diagnostic error "-Wconversion"
#endif

BEGIN_DECLARE_MODULE(FieldSideDetector)
  REQUIRE(GoalPercept)
  REQUIRE(Image)
  REQUIRE(CameraMatrix)
  REQUIRE(ArtificialHorizon)

  PROVIDE(FieldSidePercept)
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
    bool dtreeTrained;
    std::string modelFileName;
};

#endif // FIELDSIDEDETECTOR_H
