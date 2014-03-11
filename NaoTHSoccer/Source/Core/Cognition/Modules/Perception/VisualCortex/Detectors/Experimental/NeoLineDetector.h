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


#include "Tools/DoubleCamHelpers.h"
#include <Tools/DataStructures/ParameterList.h>
#include "Tools/naoth_opencv.h"
#include <algorithm>
#include <set>

//#include <gco/GCoptimization.h>


class EdgelCluster
{
public:
  struct EdgelLineComperator {
    const Math::Line& line;
    EdgelLineComperator(const Math::Line& line) : line(line) {}
    bool operator() (const Edgel& one, const Edgel& two) {
      return line.project(one.point) > line.project(two.point);
    }
  };

  struct EdgelComperator {
    bool operator() (const Edgel& one, const Edgel& two) {
      Math::Line line(one.point, Vector2d(1,0).rotate(one.angle));
      return line.project(two.point) > 0;
    }
    bool operator() (const ScanLineEdgelPercept::EdgelPair& one, const ScanLineEdgelPercept::EdgelPair& two) {
      return one.id < two.id;
    }
  };

private:
  int _id;
  
  typedef ScanLineEdgelPercept::EdgelPair Element;
  typedef std::set<Element, EdgelComperator> EdgelContainer;

  // this stuff is calcuated on demand (even inside const objects)
  mutable bool recalculate_line;
  mutable Math::Line line;
  //mutable std::vector<Edgel> edgels;
  EdgelContainer edgels;

public:
  bool inside_the_field;

public:
  EdgelCluster()
    :
    _id(-1),
    recalculate_line(false),
    inside_the_field(false)
  {
  }

  EdgelCluster(int id)
    :
    _id(id),
    recalculate_line(false),
    inside_the_field(false)
  {
  }

  ~EdgelCluster(){}

  int id(){ return _id; }
  size_t size(){ return edgels.size(); }
  const Vector2d& front() const { return edgels.begin()->point; }
  const Vector2d& back() const { return (--edgels.end())->point; }
  const EdgelContainer& getEdgels() const { return edgels; }

  void add(const Element& element) {
    edgels.insert(element);
    recalculate_line = true;
  }

  void add(const EdgelCluster& other)
  {
    for(EdgelContainer::const_iterator iter = other.edgels.begin(); iter != other.edgels.end(); ++iter) {
      add(*iter);
    }
    inside_the_field = inside_the_field || other.inside_the_field;
  }

  void draw()
  {
    if(edgels.empty()) { return; }

    int idx = id() % (unsigned int)ColorClasses::numOfColors;

    EdgelContainer::const_iterator one = edgels.begin();
    for(EdgelContainer::const_iterator two = ++edgels.begin(); two != edgels.end(); ++two, ++one) {
      LINE_PX((ColorClasses::Color) (idx), (int)one->point.x, (int)one->point.y, (int)two->point.x, (int)two->point.y);
    }

    CIRCLE_PX((ColorClasses::Color) (idx), (int)front().x, (int)front().y, 5);
    CIRCLE_PX((ColorClasses::Color) (idx), (int)back().x,  (int)back().y, 5);
  }

  const Math::Line& getLine() const
  {
    if(recalculate_line)
    {
      std::vector<cv::Point2f> points(edgels.size());
      int i = 0;
      for(EdgelContainer::const_iterator iter = edgels.begin(); iter != edgels.end(); ++iter)
      {
        points[i].x = (float)(*iter).point.x;
        points[i].y = (float)(*iter).point.y;
        i++;
      }
  
      // estimate the line
      cv::Vec4f line_coeffs;
      fitLine(cv::Mat(points), line_coeffs, CV_DIST_L2, 0, 0.01, 0.01);

      Vector2d v(line_coeffs[0], line_coeffs[1]);
      Vector2d x(line_coeffs[2], line_coeffs[3]);
      
      // set the line and sort the edgels
      line = Math::Line(x,v);
    }

    recalculate_line = false;
    return line;
  }
};


BEGIN_DECLARE_MODULE(NeoLineDetector)
  REQUIRE(ScanLineEdgelPercept)
  REQUIRE(ScanLineEdgelPerceptTop)
  REQUIRE(CameraInfo)
  REQUIRE(CameraInfoTop)

  REQUIRE(CameraMatrix)
  REQUIRE(CameraMatrixTop)
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

  
  DOUBLE_CAM_REQUIRE(NeoLineDetector,CameraInfo);
  DOUBLE_CAM_REQUIRE(NeoLineDetector,CameraMatrix);
  DOUBLE_CAM_REQUIRE(NeoLineDetector,ScanLineEdgelPercept);


  /*
  class SmoothLineEdgelEnergy : public GCoptimization::SmoothCostFunctor
  {
   private:
    const std::vector<DoubleEdgel>& scanLineEdgels;
   public:

    SmoothLineEdgelEnergy(const std::vector<DoubleEdgel>& scanLineEdgels)
      : scanLineEdgels(scanLineEdgels)
    {
    }

    virtual  GCoptimization::EnergyTermType compute(
      GCoptimization::SiteID p1, 
      GCoptimization::SiteID p2,
      GCoptimization::LabelID l1,
      GCoptimization::LabelID l2)
    {
      const DoubleEdgel& edgelOne = scanLineEdgels[p1];
      const DoubleEdgel& edgelTwo = scanLineEdgels[p2];

      double sim = edgelSim(edgelOne.center, edgelOne.center_angle, edgelTwo.center, edgelTwo.center_angle);
      return (l1==l2)?0:1/((1.0-sim)*(1.0-sim) + 0.00001);
    }
  };*/
};

#endif  /* _NeoLineDetector_H_ */

