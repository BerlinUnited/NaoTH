/**
 * @file ActiveGoalLocator.h
 *
 * @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 * Implementation of class ActiveGoalLocator
 */

#include "ActiveGoalLocator.h"

// Representations
//#include "Representations/Infrastructure/FrameInfo.h"
//#include "Representations/Infrastructure/FieldInfo.h"
//#include "Representations/Perception/GoalPercept.h"
//#include "Representations/Perception/CameraMatrix.h"
//#include "Representations/Motion/OdometryData.h"

//for MODIFY
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Debug/DebugModify.h"
//#include "Cognition/Modules/BehaviorControl/XABSLBehaviorControl/Symbols/MathSymbols.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/Math/Moments2.h"

//MATH
#include "Tools/Math/Probabilistics.h"
#include "Representations/Modeling/GoalModel.h"
//#include "Cognition/Modules/Modeling/SelfLocator/MonteCarloSelfLocator/SampleSet.h"
#include <cmath>

ActiveGoalLocator::ActiveGoalLocator()    :
    //canopyClustering(theSampleSet[1], parameters.thresholdCanopy),
    //canopyClustering2(theSampleSet2, parameters.thresholdCanopy),
    ccTrashBuffer(theSampleBuffer, parameters.thresholdCanopy)
{
  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_samples", "draw the sample set on field", true);
  //DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_goalCenter", "draw the center of goal on field", false);
  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_percept", "draw the goal percept on field", false);
  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_percept_buffer", "draw the buffer set on field", false);

  //DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_major_of_cluster", "", false);
  //DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_minor_of_cluster", "", false);

  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_goal_model", "", false);

  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:cluster_centers", "", false);

  goalWidth = (getFieldInfo().opponentGoalPostLeft - getFieldInfo().opponentGoalPostRight).abs();

  timeFilter = 0.000001;

  /*for (unsigned int i = 0; i < sampleSet.size(); i++) {
    sampleSet[i].likelihood = 0.01;
  }*/

  for(int i = 0; i < 10; i++) ccSamples[i].canopyClustering.setClusterThreshold(parameters.thresholdCanopy);

} //Constructor

void ActiveGoalLocator::execute() {

  // reset
  getLocalGoalModel().someGoalWasSeen = false;
  getLocalGoalModel().opponentGoalIsValid = false;
  getLocalGoalModel().ownGoalIsValid = false;

  /* SWITCH for different colored goals!
    18.02.2012
  clear cause of same colored Goals!

  // HACK begin: senity check: the model cannot handle posts of different color now

  if (getGoalPercept().getNumberOfSeenPosts() > 0)
  {
    ColorClasses::Color tmp_color_of_the_last_post = getGoalPercept().getPost(0).color;

    for(unsigned int i = 1; i < getGoalPercept().getNumberOfSeenPosts(); i++)
    {
      if(tmp_color_of_the_last_post != getGoalPercept().getPost(i).color)
        return;
    }//end for
  }//end if
  // HACK: end
  */

  STOPWATCH_START("ActiveGoalLocator");



  /* SWITCH for different colored goals!
    18.02.2012
  clear cause of same colored Goals!
  hold sampleSets

  static ColorClasses::Color lastGoalColor = ColorClasses::skyblue;

  if (getGoalPercept().getNumberOfSeenPosts() > 0 && getGoalPercept().getPost(0).color != lastGoalColor) {
    sampleSet.clear();

    if (getGoalPercept().getPost(0).color == ColorClasses::skyblue) {
      lastGoalColor = ColorClasses::skyblue;
    } else if (getGoalPercept().getPost(0).color == ColorClasses::yellow) {
      lastGoalColor = ColorClasses::yellow;
    }
  }
  */



  //TODO: clear???
  // don't update if the body state is not valid
  if (getBodyState().fall_down_state != BodyState::upright || // robot is not upright
      !(getBodyState().standByLeftFoot || getBodyState().standByRightFoot)) // no foot is on the ground
  {
      for (int i = 0; i < 10; i++)
        ccSamples[i].sampleSet.setUnValid();
  }

  //TODO: raus??
  //timeFilter += parameters.timeFilterRange * averageWeighting;


  //TODO reset likelihood
  /*for (unsigned int i = 0; i < sampleSet.size(); i++) {
    sampleSet[i].likelihood = 0.1;
  }*/

  Pose2D odometryDelta = lastRobotOdometry - getOdometryData();
  //TODO introduce size of ccSamples
  for(int i = 1; i < 10; i++)
    updateByOdometry(ccSamples[i].sampleSet, odometryDelta);

  updateByOdometry(theSampleBuffer, odometryDelta);
  lastRobotOdometry = getOdometryData();


  //clear old percepts of the buffer
  updateByFrameNumber(theSampleBuffer, 200);

  double weightingByFilter[10] = {0};

  bool noneFilterUpdated = true;
  bool oneFilterIsEmpty = false;

  //check if post-percepts available
  for (int i = 0; i < getGoalPercept().getNumberOfSeenPosts(); i++) {
    //if (getGoalPercept().getPost(i).positionReliable) {


      //check if particle matches any filter
      //TODO: Vorteil filter mit kleiner id!! ??
      for (int x = 0; x < 10; x++) {
        if(ccSamples[x].sampleSet.getIsValid()) {
          weightingByFilter[x] = getWeightingOfPerceptAngle(ccSamples[x].sampleSet, getGoalPercept().getPost(i));
        } else {
            oneFilterIsEmpty = true;
        }
        if (weightingByFilter[x]) {
          updateByGoalPerceptAngle(ccSamples[x].sampleSet, getGoalPercept().getPost(i));
          noneFilterUpdated = false;
        }

      }

      //both filters wasn't updated
      if (noneFilterUpdated) {

        //else insert the percept into the trashBuffer
        AGLBSample bufferSample;
          bufferSample.translation = getGoalPercept().getPost(i).position;
          bufferSample.color       = getGoalPercept().getPost(i).color;
          bufferSample.frameNumber = getFrameInfo().getFrameNumber();
        theSampleBuffer.samples.add(bufferSample);
      }

  }//end for i < getGoalPercept().getNumberOfSeenPosts()


  //////////////////////////
  //   check Buffer
  //////////////////////////

  //just check Buffer if one PF is empty
  //hier evtl bei mehreren filtern immer clustern!
  if (oneFilterIsEmpty)
  {
    ccTrashBuffer.cluster();

    //check if useable cluster in TrashBuffer exists
    checkTrashBuffer(theSampleBuffer);
  }//end if (!theSampleSet1.getIsValid() || !theSampleSet2.getIsValid())


  for (unsigned int i = 0; i < 10; i++) {
    resampleGT07(ccSamples[i].sampleSet, true);
  }


  /*Decision in Modeling
    oppGoalSeen = true;
    ownGoalSeen = true;
  }*/

  debugDrawings();

  STOPWATCH_STOP("ActiveGoalLocator");

}//end execute

void ActiveGoalLocator::debugDrawings() {

    DEBUG_REQUEST("ActiveGoalLocator:draw_goal_model",

      FIELD_DRAWING_CONTEXT;
        PEN("FF0000", 50);
        CIRCLE(getLocalGoalModel().goal.leftPost.x, getLocalGoalModel().goal.leftPost.y, 50);
        PEN("0000FF", 50);
        CIRCLE(getLocalGoalModel().goal.rightPost.x, getLocalGoalModel().goal.rightPost.y, 50);
        LINE(getLocalGoalModel().goal.rightPost.x, getLocalGoalModel().goal.rightPost.y, getLocalGoalModel().goal.leftPost.x, getLocalGoalModel().goal.leftPost.y);
  );

  DEBUG_REQUEST("ActiveGoalLocator:draw_samples",
  for (unsigned int x = 0; x < 10; x++ ) {

    string color = ColorClasses::colorClassToHex((ColorClasses::Color)((x+3)%ColorClasses::numOfColors));

    if (!ccSamples[x].sampleSet.getIsValid()) //alls PFs are filled initilized
        continue;

    std::cout << "Filter " << x << " is valid" <<  std::endl;

    for (unsigned int i = 0; i < ccSamples[x].sampleSet.size(); i++) {
      const AGLSample& sample = ccSamples[x].sampleSet[i];

        FIELD_DRAWING_CONTEXT;
        PEN(color, 30);
        CIRCLE(sample.translation.x, sample.translation.y, 20);
        TEXT_DRAWING(sample.translation.x+10,sample.translation.y+10, x);
    }
  });

  DEBUG_REQUEST("ActiveGoalLocator:draw_percept",
  for (int i = 0; i < getGoalPercept().getNumberOfSeenPosts(); i++) {
    const Vector2<double> percept = getGoalPercept().getPost(i).position;

      FIELD_DRAWING_CONTEXT;
      PEN("FF0000", 30);
      CIRCLE(percept.x, percept.y, 20);

  });


  DEBUG_REQUEST("ActiveGoalLocator:draw_percept_buffer",

    //std::cout << "size" << theSampleBuffer.size() << std::endl;
    for (unsigned int i = 0; i < theSampleBuffer.size(); i++) {
      const AGLBSample& sample = theSampleBuffer[i];

      FIELD_DRAWING_CONTEXT;
      PEN("000000", 50);
      CIRCLE(sample.translation.x, sample.translation.y, 20);

      //std::cout << "it" << i << "x" << sample.translation.x << "y" << sample.translation.y << std::endl;
  });

} //end debugDrawings

void ActiveGoalLocator::updateByOdometry(AGLSampleSet& sampleSet, const Pose2D& odometryDelta) const
{
    for (unsigned int i = 0; i < sampleSet.size(); i++) {
      //update each particle with odometry
      sampleSet[i].translation = odometryDelta * sampleSet[i].translation;

      //making noise
      sampleSet[i].translation.x += (Math::random()-0.5)*parameters.motionNoiseDistance;
      sampleSet[i].translation.y += (Math::random()-0.5)*parameters.motionNoiseDistance;
    }

}//end updateByOdometry

void ActiveGoalLocator::updateByOdometry(AGLSampleBuffer& sampleSet, const Pose2D& odometryDelta) const
{

    for (unsigned int i = 0; i < sampleSet.size(); i++) {
      //update each particle with odometry
      sampleSet[i].translation = odometryDelta * sampleSet[i].translation;

      //making noise
      sampleSet[i].translation.x += (Math::random()-0.5)*parameters.motionNoiseDistance;
      sampleSet[i].translation.y += (Math::random()-0.5)*parameters.motionNoiseDistance;
    }

}//end updateByOdometry

double ActiveGoalLocator::getWeightingOfPerceptAngle(const AGLSampleSet& sampleSet, const GoalPercept::GoalPost& post) {
  //TODO check Color -> not similar, return 0
  double weighting(0.0);
  for (unsigned int i = 0; i < sampleSet.size(); i++) {
    double value = Math::normalize(sampleSet[i].getPos().angle() - post.position.angle()); //normalisieren, sonst pi--pi zu groß! value kann auch neg sein, da nur als norm in normalverteilung
    weighting = Math::gaussianProbability(value, parameters.standardDeviationAngle);
  }

  if (weighting > parameters.weightingTreshholdForUpdateWithAngle)//TODO: check value
    return weighting;
  else
    return 0;
}

//TODO side effect with ccSamples los werdn
void ActiveGoalLocator::checkTrashBuffer(AGLSampleBuffer& sampleBuffer)
{

  if (ccTrashBuffer.getLargestCluster().size() > 9) //param
  {

      for (int i = 0; i < 10; i++) {

          if (!ccSamples[i].sampleSet.getIsValid()) {
            initFilterByBuffer(ccTrashBuffer.getLargestClusterID(), sampleBuffer, ccSamples[i].sampleSet);
            break; //xxx
          }  //end if ccSamples[i] is empty
      }


  }//if largestCluster > 0

}// checkTrashBuffer

void ActiveGoalLocator::initFilterByBuffer(const int& largestClusterID, AGLSampleBuffer& sampleSetBuffer, AGLSampleSet& sampleSet)
{

  //already known that sampleSet is empty!
  int n = 0;
  for (unsigned int i = 0; i < sampleSetBuffer.size(); i++) {

    //search all particles with ID of largest cluster and add them
    //TODO make n to param
    //&& largestClusterID > -1
    if (n < sampleSetBuffer.samples.getNumberOfEntries() && n < (int)sampleSet.numberOfParticles) {//  && sampleSetBuffer.samples.getEntry(i).cluster == largestClusterID) {
      AGLSample tmpSample;
      tmpSample.color = sampleSetBuffer[i].color;
      tmpSample.translation = sampleSetBuffer[i].getPos();
      tmpSample.likelihood = 1.0/(double)sampleSet.size();
      sampleSet[n] = tmpSample;
      n++;
      //std::cout << tmpSample.likelihood << std::endl;
      //std::cout << sampleSet[n].likelihood << std::endl;

    } //else { FIXME
      //would be nice to hold the samples we don't have clustert
      //tmpSampleBuffer.samples.add(tmpSampleBuffer.samples.getEntry(i));
    //}
  }
  sampleSet.setValid();
  //not just clear completly??
  theSampleBuffer.samples.clear();


}//copyFromBufferToFilter

void ActiveGoalLocator::updateByFrameNumber(AGLSampleBuffer& sampleSet, const unsigned int frames) const
{
    if(sampleSet.samples.getNumberOfEntries() > 0 &&
     getFrameInfo().getFrameNumber() - sampleSet.samples.first().frameNumber > frames)
    {
        sampleSet.samples.removeFirst();
    }
}


void ActiveGoalLocator::updateByGoalPerceptAngle(AGLSampleSet& sampleSet, const GoalPercept::GoalPost& post) {


  for (unsigned int i = 0; i < sampleSet.size(); i++) {;

    double weighting(0.0);

        double diff = Math::normalize(sampleSet[i].getPos().angle() - post.position.angle()); //normalisieren, sonst pi--pi zu groß! value kann auch neg sein, da nur als norm in normalverteilung
        weighting = Math::gaussianProbability(diff, parameters.standardDeviationAngle);

        std::cout << weighting << std::endl;


    sampleSet[i].likelihood *= weighting;

  }//end for


}//end updateByBallPercept

void ActiveGoalLocator::resampleGT07(AGLSampleSet& sampleSet, bool noise) {

  double totalWeighting = 0;

  //Should also be calculated while "inserting"? -> NO ... better independent from update or insert!
  for (unsigned int i = 0; i < sampleSet.size(); i++) {
    totalWeighting += sampleSet[i].likelihood;
  }//end for

  MODIFY("ActiveGoalLocator:totalWeighting", totalWeighting); //justDEBUG ERROR!!

  if (sampleSet.getIsValid()) //TODO: nicht leer!??
    averageWeighting = totalWeighting / (double) sampleSet.size();
  else averageWeighting = 0.0001234;

  //PLOT("ActiveGoalLocator:averageWeighting", averageWeighting);
  MODIFY("ActiveGoalLocator:averageWeighting", averageWeighting); //justDEBUG

  // copy the samples
  // TODO: use memcopy?
  AGLSampleSet oldSampleSet = sampleSet;

  totalWeighting += parameters.resamplingThreshhold * oldSampleSet.size();
  for (unsigned int i = 0; i < oldSampleSet.size(); i++) {
    oldSampleSet[i].likelihood += parameters.resamplingThreshhold;
    oldSampleSet[i].likelihood /= totalWeighting; // normalize
  }//end for


  // resample 10% of particles
  //int numberOfPartiklesToResample = (int) (((double) sampleSet.size())*0.1 + 0.5);

  double sum = -Math::random();
  unsigned int count = 0;

  //unsigned int m = 0; // Zaehler durchs Ausgangs-Set
  unsigned int n = 0; // Zaehler durchs Ziel-Set

  for (unsigned m = 0; m < sampleSet.size(); m++) {
    sum += oldSampleSet[m].likelihood * oldSampleSet.size();

    // select the particle to copy
    while (count < sum && count < oldSampleSet.size()) {
      if (n >= oldSampleSet.size()) break;

      // copy the selected particle
      sampleSet[n] = oldSampleSet[m];
      if (noise) {
        sampleSet[n].translation.x += (Math::random() - 0.5) * parameters.processNoiseDistance;
        sampleSet[n].translation.y += (Math::random() - 0.5) * parameters.processNoiseDistance;
      }

      n++;
      count++;

    }//end while
  }//end for

}//end resampleGT07

