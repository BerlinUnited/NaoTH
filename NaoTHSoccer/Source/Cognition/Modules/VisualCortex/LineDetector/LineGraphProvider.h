/* 
 * File:   LineGraphProvider.h
 * Author: Heinrich Mellmann
 *
 */

#ifndef _LineGraphProvider_H_
#define _LineGraphProvider_H_

#include <ModuleFramework/Module.h>

// Tools
#include "Tools/ColorClasses.h"
#include "Tools/Math/Vector2.h"

#include "Tools/ImageProcessing/Edgel.h"
#include "Tools/Math/Line.h"



// Representations
#include "Representations/Perception/ScanLineEdgelPercept.h"
#include "Representations/Infrastructure/CameraInfo.h"
#include "Representations/Infrastructure/FieldInfo.h"
#include "Representations/Perception/CameraMatrix.h"
#include "Representations/Modeling/OdometryData.h"
#include "Representations/Modeling/ProbabilisticQuadCompas.h"
#include "Representations/Perception/LineGraphPercept.h"

#include "Tools/DoubleCamHelpers.h"
#include <algorithm>
#include <set>

#include "Representations/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugImageDrawings.h"
#include "Tools/Debug/DebugParameterList.h"

// test
#include "Tools/icp/icpPointToPlane.h"
#include "Tools/icp/icpPointToPoint.h"

BEGIN_DECLARE_MODULE(LineGraphProvider)
  PROVIDE(StopwatchManager)
  PROVIDE(DebugRequest)
  PROVIDE(DebugModify)
  PROVIDE(DebugDrawings)
  PROVIDE(DebugImageDrawings)
  PROVIDE(DebugImageDrawingsTop)
  PROVIDE(DebugParameterList)

  REQUIRE(FieldInfo)

  REQUIRE(ScanLineEdgelPercept)
  REQUIRE(ScanLineEdgelPerceptTop)
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)

  REQUIRE(OdometryData)

  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)

  PROVIDE(ProbabilisticQuadCompas)
  PROVIDE(LineGraphPercept)
END_DECLARE_MODULE(LineGraphProvider)


class LineGraphProvider : private LineGraphProviderBase
{
public:
  LineGraphProvider();
  virtual ~LineGraphProvider();

  virtual void execute(CameraInfo::CameraID id);

  virtual void execute()
  {
    getProbabilisticQuadCompas().reset();
    getLineGraphPercept().edgels.clear();

    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);
    
    getProbabilisticQuadCompas().normalize();


    DEBUG_REQUEST("Vision:LineGraphProvider:draw_lines_point_model",
      FIELD_DRAWING_CONTEXT;
      PEN("000000", 3);
      for(size_t i = 0; i < lines_point_model.size(); i += 2) {
        CIRCLE(lines_point_model[i], lines_point_model[i+1], 3);
      }
    );

    DEBUG_REQUEST("Vision:LineGraphProvider:draw_LineGraphPercept",
      FIELD_DRAWING_CONTEXT;
      PEN("0000FF", 5);
      
      for(size_t i = 0; i < getLineGraphPercept().edgels.size(); i++)
      {
        const Edgel& e = getLineGraphPercept().edgels[i];
        CIRCLE(e.point.x, e.point.y, 10);
      }
    );


    if(getLineGraphPercept().edgels.size() > 10) 
    {
      
      lines_point_samples.clear();
      for(size_t i = 0; i < getLineGraphPercept().edgels.size(); i++)
      {
        const Edgel& e = getLineGraphPercept().edgels[i];
        lines_point_samples.push_back(e.point.x);
        lines_point_samples.push_back(e.point.y);
      }

      Matrix R = Matrix::eye(2);
      Matrix t(2,1);
      double minError = -1;

      for(int i = 0; i < 100; i++) {
        
        Pose2D sample;
        sample.translation.x = Math::random(getFieldInfo().carpetRect.min().x, getFieldInfo().carpetRect.max().x);
        sample.translation.y = Math::random(getFieldInfo().carpetRect.min().y, getFieldInfo().carpetRect.max().y);
        sample.rotation = Math::random(-Math::pi, Math::pi);
        
        Matrix R_ = Matrix::eye(2);
        R_.val[0][0] = R_.val[1][1] = cos(sample.rotation);
        R_.val[1][0] = sin(sample.rotation);
        R_.val[0][1] = -sin(sample.rotation);
        
        Matrix t_(2,1);
        t_.val[0][0] = sample.translation.x;
        t_.val[1][0] = sample.translation.y;
        
        icp->fit(lines_point_samples.data(),lines_point_samples.size()/2,R_,t_,-1);

        if(minError == -1 || minError > icp->minError) {
          minError = icp->minError;
          R = R_;
          t = t_;
        }
      }

      double r00 = R.val[0][0]; double r01 = R.val[0][1];
      double r10 = R.val[1][0]; double r11 = R.val[1][1];
      double t0  = t.val[0][0]; double t1  = t.val[1][0];

      DEBUG_REQUEST("Vision:LineGraphProvider:draw_lines_point_samples_transform",
        FIELD_DRAWING_CONTEXT;

        //ROBOT(pose.translation.x, pose.translation.y, pose.rotation);

        PEN("FF0000", 10);

        for(size_t i = 0; i < lines_point_samples.size(); i += 2)
        {
          double x = (float)(r00*lines_point_samples[i+0] + r01*lines_point_samples[i+1] + t0);
          double y = (float)(r10*lines_point_samples[i+0] + r11*lines_point_samples[i+1] + t1);

          CIRCLE(x, y, 10);
        }
      );
    }
  }

  class Parameters: public ParameterList
  {
  public:
    Parameters() : ParameterList("LineGraphProvider")
    {
      PARAMETER_REGISTER(edgelSimThreshold) = 0.8;
      PARAMETER_REGISTER(quadCompasSmoothingFactor) = 0.4;
      PARAMETER_REGISTER(minimalNumberOfPairs) = 0;

      syncWithConfig();
    }

    virtual ~Parameters() {
    }

    double edgelSimThreshold;
    double quadCompasSmoothingFactor;
    int minimalNumberOfPairs;
  } parameters;

  struct Neighbors {
    Neighbors():left(-1), right(-1), w_left(0), w_right(0){}
    int left;
    int right;
    double w_left;
    double w_right;
    int operator[](int i){ return i==0?left:right; }
    void reset() {left = right= -1; w_left = w_right = 0; }
  };
  
  struct EdgelPair {
    int left;
    int right;
    double sim;
    EdgelPair(int left, int right, double sim) : left(left), right(right), sim(sim) {}
  };

private: // data members
  CameraInfo::CameraID cameraID;

  std::vector<Neighbors> edgelNeighbors;
  std::vector<EdgelPair> edgelPairs;
  std::vector<Vector2d> edgelProjections;

private: // test
  std::vector<double> lines_point_model;
  std::vector<double> lines_point_samples;
  IcpPointToPoint* icp;
  void test_icp() const;

private: // method members
  static double edgelSim(const EdgelT<double>& e1, const EdgelT<double>& e2);

  void calculatePairsAndNeigbors(
    const std::vector<ScanLineEdgelPercept::EdgelPair>& edgels, 
    std::vector<EdgelPair>& edgelPairs, 
    std::vector<Neighbors>& neighbors, double threshold) const;

  DOUBLE_CAM_PROVIDE(LineGraphProvider, DebugImageDrawings);

  DOUBLE_CAM_REQUIRE(LineGraphProvider, CameraInfo);
  DOUBLE_CAM_REQUIRE(LineGraphProvider, CameraMatrix);
  DOUBLE_CAM_REQUIRE(LineGraphProvider, ScanLineEdgelPercept);
};

#endif  /* _LineGraphProvider_H_ */

