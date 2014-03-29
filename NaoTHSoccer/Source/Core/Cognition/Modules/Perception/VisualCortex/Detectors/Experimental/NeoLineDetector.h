/* 
 * File:   NeoLineDetector.h
 * Author: Heinrich Mellmann
 *
 */

#ifndef _NeoLineDetector_H_
#define _NeoLineDetector_H_

#include <ModuleFramework/Module.h>

// Tools
#include "Tools/ColorClasses.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugParameterList.h"

#include "Tools/ImageProcessing/Edgel.h"
#include "Tools/Math/Line.h"

// Representations
#include "Representations/Perception/ScanLineEdgelPercept.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Perception/CameraMatrix.h"

#include "Representations/Modeling/OdometryData.h"

#include "Representations/Modeling/ProbabilisticQuadCompas.h"


#include "Tools/DoubleCamHelpers.h"
#include <Tools/DataStructures/ParameterList.h>
#include "Tools/naoth_opencv.h"
#include <algorithm>
#include <set>

#include "EdgelCluster.h"

BEGIN_DECLARE_MODULE(NeoLineDetector)
  REQUIRE(ScanLineEdgelPercept)
  REQUIRE(ScanLineEdgelPerceptTop)
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)

  REQUIRE(OdometryData)

  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)

  PROVIDE(ProbabilisticQuadCompas);
END_DECLARE_MODULE(NeoLineDetector)


class NeoLineDetector : private NeoLineDetectorBase
{
public:
  NeoLineDetector();
  virtual ~NeoLineDetector();

  virtual void execute(CameraInfo::CameraID id);

  void execute()
  {
    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);
  }

  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("NeoLineDetector")
    {
      PARAMETER_REGISTER(x) = 6;

      syncWithConfig();

      DebugParameterList::getInstance().add(this);
    }

    ~Parameters() {
      DebugParameterList::getInstance().remove(this);
    }

    int x;
  } theParameters;

private:
  CameraInfo::CameraID cameraID;

  struct Neighbors {
    Neighbors():left(-1), right(-1), w_left(0), w_right(0){}
    int left;
    int right;
    double w_left;
    double w_right;
    int operator[](int i){ return i==0?left:right; }
    void reset() {left = right= -1; w_left = w_right = 0; }
  };
  
  static double edgelSim(const Vector2d& p1, double a1, const Vector2d& p2, double a2);
  static double edgelSim(const EdgelT<double>& e1, const EdgelT<double>& e2);
  static double segmentSim(const Vector2d& p0, const Vector2d& p1, const Vector2d& q0, const Vector2d& q1);

  struct EdgelPair {
    int left;
    int right;
    double sim;
    EdgelPair(int left, int right, double sim) : left(left), right(right), sim(sim) {}
  };

  struct Compare_double_pair {
    bool operator() (EdgelPair one, EdgelPair two) { 
      return one.sim > two.sim;
    }
  } compare_double_pair;


private:
  std::vector<Neighbors> edgelNeighbors;
  std::vector<EdgelPair> edgelPairs;
  std::vector<Vector2d> edgelProjections;

  void calculateNeigbors(const std::vector<ScanLineEdgelPercept::EdgelPair>& edgels, std::vector<Neighbors>& neighbors, double threshold) const;
  void calculatePairs(const std::vector<ScanLineEdgelPercept::EdgelPair>& edgels, std::vector<EdgelPair>& edgelPairs, double threshold) const;
  void calculatePairsAndNeigbors(
    const std::vector<ScanLineEdgelPercept::EdgelPair>& edgels, 
    std::vector<EdgelPair>& edgelPairs, 
    std::vector<Neighbors>& neighbors, double threshold) const;

  
  
  DOUBLE_CAM_REQUIRE(NeoLineDetector,CameraInfo);
  DOUBLE_CAM_REQUIRE(NeoLineDetector,CameraMatrix);
  DOUBLE_CAM_REQUIRE(NeoLineDetector,ScanLineEdgelPercept);
};

#endif  /* _NeoLineDetector_H_ */

