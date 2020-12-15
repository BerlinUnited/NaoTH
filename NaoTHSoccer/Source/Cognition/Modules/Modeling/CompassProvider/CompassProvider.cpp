/* 
 * File:   CompassProvider.cpp
 * Author: Heinrich Mellmann
 */

#include "CompassProvider.h"

#include "Tools/CameraGeometry.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Math/Geometry.h"

CompassProvider::CompassProvider()
{
  DEBUG_REQUEST_REGISTER("Vision:CompassProvider:draw_compas", "draw compas direcion based on the edgel directions", false);

  getDebugParameterList().add(&params);
}


CompassProvider::~CompassProvider()
{
  getDebugParameterList().remove(&params);
}


void CompassProvider::execute()
{
  getProbabilisticQuadCompas().reset();

  /*
  // calculate the projection for all edgels
  edgelProjectionsBegin.resize(getScanLineEdgelPercept().pairs.size());
  edgelProjectionsEnd.resize(getScanLineEdgelPercept().pairs.size());
  for(size_t i = 0; i < getScanLineEdgelPercept().pairs.size(); i++) 
  {  
    const ScanLineEdgelPercept::EdgelPair& pair = getScanLineEdgelPercept().pairs[i];

    const Edgel& end = getScanLineEdgelPercept().edgels[pair.end];
    const Edgel& begin = getScanLineEdgelPercept().edgels[pair.begin];
    
    // NOTE: edgels are assumed to be always below horizon and so the projection should be allways valid
    CameraGeometry::imagePixelToFieldCoord(
      getCameraMatrix(), getCameraInfo(),
      end.point.x,
      end.point.y,
      0.0,
      edgelProjectionsEnd[i]);

    CameraGeometry::imagePixelToFieldCoord(
      getCameraMatrix(), getCameraInfo(),
      begin.point.x,
      begin.point.y,
      0.0,
      edgelProjectionsBegin[i]);

    //pair.projectedWidth = Vector2d(beginPointOnField - endPointOnField).abs();
  }
  */

  ASSERT(getRansacLinePercept().edgelLineIDs.size() == getLineGraphPercept().edgelsOnField.size());
  // fill the compas
  if((int)getLineGraphPercept().edgelsOnField.size() > params.minimalNumberOfPairs)
  {
    getProbabilisticQuadCompas().setSmoothing(params.quadCompasSmoothingFactor);
    for(size_t j = 0; j < getLineGraphPercept().edgelsOnField.size(); ++j)
    {
      //const EdgelPair& edgelPair = edgelPairs[j];
      //const Vector2d& edgelLeft = edgelProjections[edgelPair.left];
      //const Vector2d& edgelRight = edgelProjections[edgelPair.right];

      // TODO: mean difference?
      //double r = (edgelProjectionsBegin[edgelPair.left] - edgelProjectionsBegin[edgelPair.right]).angle();

      if(getRansacLinePercept().edgelLineIDs[j] > -1) {
        double r = getLineGraphPercept().edgelsOnField[j].direction.angle();
        //getProbabilisticQuadCompas().add(r, edgelPair.sim);
        getProbabilisticQuadCompas().add(r);
      }
    }
  }


  getProbabilisticQuadCompas().normalize();


  DEBUG_REQUEST("Vision:CompassProvider:draw_compas",
      
      Vector2d last;
      double last_v = 0;

      double scale = 5000;
      double offset = 150;

      FIELD_DRAWING_CONTEXT;
      for(size_t x = 0; x < getProbabilisticQuadCompas().size()*4+1; x++)
      {
        double v = getProbabilisticQuadCompas()[x];
        Vector2d a(offset + v*scale, 0.0);
        a.rotate(Math::fromDegrees(((double)x)*5.0));
        if(x > 0) {

          double d = Math::clamp(std::min(v, last_v)/0.1, 0.0, 1.0);
          Color c(0.0, d, 1-d);
          PEN(c, 10);

          LINE(last.x,last.y,a.x,a.y);
        }
        last = a;
        last_v = v;
      }
  );

}//end execute
