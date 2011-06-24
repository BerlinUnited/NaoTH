/**
* @file ParticleFilterBallLocator.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class ParticleFilterBallLocator
*/

#include "ParticleFilterBallLocator.h"

// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Debug/DebugDrawings3D.h"
#include "Tools/Debug/DebugImageDrawings.h"

// Tools
#include "Tools/Math/Probabilistics.h"
#include "Tools/Math/Geometry.h"


ParticleFilterBallLocator::ParticleFilterBallLocator()
{
  DEBUG_REQUEST_REGISTER("ParticleFilterBallLocator:draw_ball_on_field", "draw the ball model on the field", false);
  DEBUG_REQUEST_REGISTER("ParticleFilterBallLocator:draw_samples", "draw the sample set", false);
}

void ParticleFilterBallLocator::execute()
{
  getBallModel().reset();
  
  updateByBallPercept();
  
  DEBUG_REQUEST("ParticleFilterBallLocator:draw_ball_on_field", drawBallModel(getBallModel()); );
  DEBUG_REQUEST("ParticleFilterBallLocator:draw_samples", drawSamples(sampleSet); );
}//end execute


void ParticleFilterBallLocator::updateByBallPercept()
{

  // integrate new percepts
  if(sampleSet.size() < 100)
  {
    Sample s;
    s.position = getBallPercept().bearingBasedOffsetOnField;
    sampleSet.push_back(s);
  }
  else
  {
    int idx = Math::random(100);
    sampleSet[idx].position = getBallPercept().bearingBasedOffsetOnField;
  }
}//end updateByBallPercept


void ParticleFilterBallLocator::resampleGT07(bool noise)
{
  // parameters
  double processNoiseDistance = 5;
  MODIFY("ParticleFilterBallLocator:processNoiseDistance",processNoiseDistance);
  double resamplingThreshhold = 0.0;
  MODIFY("ParticleFilterBallLocator:resamplingThreshhold",resamplingThreshhold);

  // 
  double totalWeighting = 0;
  for(unsigned int i = 0; i < sampleSet.size(); i++)
  {
    totalWeighting += sampleSet[i].likelihood;
  }//end for
  const double averageWeighting = totalWeighting / (double)sampleSet.size();
  PLOT("ParticleFilterBallLocator:averageWeighting", averageWeighting);

  // copy the samples 
  // TODO: use memcopy?
  std::vector<Sample> oldSampleSet = sampleSet;
  
  totalWeighting += resamplingThreshhold*oldSampleSet.size();
  for(unsigned int i = 0; i < oldSampleSet.size(); i++)
  {
    oldSampleSet[i].likelihood += resamplingThreshhold;
    oldSampleSet[i].likelihood /= totalWeighting; // normalize
  }//end for


  // resample 10% of particles
  int numberOfPartiklesToResample = (int)(((double)sampleSet.size())*0.1+0.5);
 
  double sum = -Math::random();
  unsigned int count = 0;

  unsigned int m = 0;  // Zaehler durchs Ausgangs-Set
  unsigned int n = 0;  // Zaehler durchs Ziel-Set

  for(unsigned m = 0; m < sampleSet.size(); m++)
  {
    sum += oldSampleSet[m].likelihood * oldSampleSet.size();

    // select the particle to copy
    while(count < sum && count < oldSampleSet.size())
    {
      if (n >= oldSampleSet.size()) break;
      
      // copy the selected particle
      sampleSet[n] = oldSampleSet[m];
      if(noise)
      {
        sampleSet[n].position.x += (Math::random()-0.5)*processNoiseDistance;
        sampleSet[n].position.y += (Math::random()-0.5)*processNoiseDistance;
      }
      
      n++;
      count++;
    }//end while
  }//end for

}//end resampleGT07


void ParticleFilterBallLocator::drawSamples(const SampleSet& sampleSet) const
{
  FIELD_DRAWING_CONTEXT;

  // plot samples
  for (unsigned int i = 0; i < sampleSet.size(); i++)
  {
    if(sampleSet[i].moving)
      PEN("FF9900", 20);
    else
      PEN("0099FF", 20);

    const Vector2<double>& pos = sampleSet[i].position;
    FILLOVAL( pos.x, pos.y, 10, 10);

    if(sampleSet[i].moving)
    {
      const Vector2<double>& speed = sampleSet[i].speed;
      ARROW( pos.x, pos.y, pos.x + speed.x, pos.y + speed.y);
    }//end if
  }//end for
}//end drawSamples


void ParticleFilterBallLocator::drawBallModel(const BallModel& ballModel) const
{
  FIELD_DRAWING_CONTEXT;
  
  if(ballModel.valid)
  {
    if(ballModel.ballWasSeen)
      PEN("FF9900", 20);
    else
      PEN("0099FF", 20);
  }
  else
    PEN("999999", 20);

  CIRCLE( ballModel.position.x, ballModel.position.y, getFieldInfo().ballRadius-10);
  ARROW( ballModel.position.x, ballModel.position.y,
         ballModel.position.x+ballModel.speed.x, 
         ballModel.position.y+ballModel.speed.y);

  // draw the distribution
  /*
  PEN("00FFFF", 20);
  OVAL(getBallModel().position.x, getBallModel().position.y, Px[0][0], Py[0][0]);
  PEN("FF00FF", 20);
  OVAL(getBallModel().position.x, getBallModel().position.y, Px[1][1], Py[1][1]);
  */

  PEN("999999", 10);
  for(int i=1; i <= BALLMODEL_MAX_FUTURE_SECONDS; i++)
  {
    CIRCLE(ballModel.futurePosition[i].x, ballModel.futurePosition[i].y, getFieldInfo().ballRadius-5);
  }
}//end drawBallModel

