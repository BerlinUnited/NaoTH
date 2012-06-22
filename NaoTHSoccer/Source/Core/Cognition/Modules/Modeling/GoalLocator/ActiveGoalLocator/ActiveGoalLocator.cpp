/**
 * @file ActiveGoalLocator.h
 *
 * @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 * Implementation of class ActiveGoalLocator
 */

#include "ActiveGoalLocator.h"


//for MODIFY
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/Math/Moments2.h"

//MATH
#include "Tools/Math/Probabilistics.h"
#include "Representations/Modeling/GoalModel.h"
#include <cmath>

ActiveGoalLocator::ActiveGoalLocator()    :
    //canopyClustering(theSampleSet[1], parameters.thresholdCanopy),
    //canopyClustering2(theSampleSet2, parameters.thresholdCanopy),
    ccTrashBuffer(theSampleBuffer, parameters.thresholdCanopy)
{
  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_samples", "draw the sample set on field", true);
  //DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_goalCenter", "draw the center of goal on field", false);
  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_percept", "draw the goal percept on field", false);
  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_percept_buffer", "draw the buffer set on field", true);

  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_goal_model", "", false);

  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_mean_of_each_valid_PF", "", true);

  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:which_filter_are_valid_to_StdOut", "Print the valid PFs in each time frame to check which is valid", true);

  goalWidth = (getFieldInfo().opponentGoalPostLeft - getFieldInfo().opponentGoalPostRight).abs();

  for (unsigned int x = 0; x < 10; x++) {
    for (unsigned int i = 0; i < ccSamples[x].sampleSet.size(); i++) {
      ccSamples[x].sampleSet[i].likelihood = 0.1; //param
    }
  }

  for(int i = 0; i < 10; i++) ccSamples[i].canopyClustering.setClusterThreshold(parameters.thresholdCanopy);

} //Constructor

void ActiveGoalLocator::execute() {

  STOPWATCH_START("ActiveGoalLocator");

  // don't update if the body state is not valid
  if (getBodyState().fall_down_state != BodyState::upright || // robot is not upright
      !(getBodyState().standByLeftFoot || getBodyState().standByRightFoot)) // no foot is on the ground
  {
      for (int i = 0; i < 10; i++)
        ccSamples[i].sampleSet.setUnValid();

      return;
  }

  //decrease lastTotalWeighting of each PF

  //TODO filter decreasen
  for (unsigned int x = 0; x < 10; x++) {
    if (ccSamples[x].sampleSet.getIsValid()) {
        ccSamples[x].sampleSet.lastTotalWeighting *= parameters.timeFilterRange;

        if (ccSamples[x].sampleSet.lastTotalWeighting < parameters.deletePFbyTotalWeightingThreshold) {
          ccSamples[x].sampleSet.setUnValid();
          std::cout << "delete filter " << x << "because " << ccSamples[x].sampleSet.lastTotalWeighting << " is smaler " << parameters.deletePFbyTotalWeightingThreshold << std::endl;
        }
     }
  }

  //TODO reset likelihood of valid filter
  for (unsigned int x = 0; x < 10; x++) {
    if (ccSamples[x].sampleSet.getIsValid()) {
      for (unsigned int i = 0; i < ccSamples[x].sampleSet.size(); i++) {
        ccSamples[x].sampleSet[i].likelihood = 0.1;
      }
    }
  }

  //////////////////////////
  //   check Odometry
  //////////////////////////

  Pose2D odometryDelta = lastRobotOdometry - getOdometryData();
  //TODO introduce size of ccSamples
  for(int i = 1; i < 10; i++)
    updateByOdometry(ccSamples[i].sampleSet, odometryDelta);

  updateByOdometry(theSampleBuffer, odometryDelta);
  lastRobotOdometry = getOdometryData();

  updateByFrameNumber(theSampleBuffer, 200); //clear old percepts

  //////////////////////////
  //   check Percepts
  //////////////////////////

  double weightingByFilter[10] = {0};

  bool noneFilterUpdated = true;
  bool oneFilterIsEmpty = false;

  //check if post-percepts available
  for (int i = 0; i < getGoalPercept().getNumberOfSeenPosts(); i++) {

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
      if (noneFilterUpdated && getGoalPercept().getPost(i).positionReliable) { //buffer just store reliable posts, because they are later clustered by position

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
  if (oneFilterIsEmpty)
  {
    ccTrashBuffer.cluster();
    checkTrashBuffer(theSampleBuffer); //check if useable cluster in TrashBuffer exists and insert
  }

  //TODO check if buffer possible, then clear one filter!!!

  for (unsigned int i = 0; i < 10; i++) {
    resampleGT07(ccSamples[i].sampleSet, true);
  }

  //calculate mean for all valid PFs
  for(unsigned int x = 0; x < 10; x++) {

    Vector2<double> mean;

    if (ccSamples[x].sampleSet.getIsValid()) {

      for (unsigned int i = 0; i < ccSamples[x].sampleSet.size(); i++) {
        mean += ccSamples[x].sampleSet[i].getPos();

      }
       ccSamples[x].sampleSet.mean = mean/(double)ccSamples[x].sampleSet.size();
    }
  }

  //////////////////////////
  //   provide Goal Model
  //////////////////////////

  getLocalGoalModel().opponentGoalIsValid = false;
  getLocalGoalModel().ownGoalIsValid = false;
  getLocalGoalModel().someGoalWasSeen = false;

  //TODO Make as member
  unsigned int numOfValidFilter = 0;
  for(unsigned int i = 0; i < 10; i++) {
    if (ccSamples[i].sampleSet.getIsValid())
      numOfValidFilter++;
  }

  if (numOfValidFilter > 1) {

    unsigned int id1 = 0;
    unsigned int id2 = 0;
    double lastDistError = 1000000;

    for(unsigned int x = 0; x < 10; x++) {

      double distError = 0;

      if (ccSamples[x].sampleSet.getIsValid()) {

        for(unsigned int i = 0; i < 10; i++) {

            if (ccSamples[i].sampleSet.getIsValid()) {

                distError = abs((ccSamples[x].sampleSet.mean - ccSamples[i].sampleSet.mean).abs() - goalWidth);

                if (distError < lastDistError) { //check if error becomes better
                    lastDistError = distError;
                    id1 = x;
                    id2 = i;
                }
            }//end if
        }//end for i
      }//end if
    }//end for x

    //decide whether distError is feasable
    if (lastDistError < parameters.possibleGoalWidhtError) {

      if (ccSamples[id1].sampleSet.mean.angle() < ccSamples[id2].sampleSet.mean.angle()) {
        getLocalGoalModel().goal.leftPost  = ccSamples[id1].sampleSet.mean;
        getLocalGoalModel().goal.rightPost = ccSamples[id2].sampleSet.mean;
      } else {
        getLocalGoalModel().goal.leftPost  = ccSamples[id2].sampleSet.mean;
        getLocalGoalModel().goal.rightPost = ccSamples[id1].sampleSet.mean;
      }

      getLocalGoalModel().someGoalWasSeen = true;
      //frame Info when goal was seen not useful! New: some_goal_was seen
      getLocalGoalModel().goal.frameInfoWhenGoalLastSeen = getFrameInfo();

    }

  }//end if numOfValidFilter

  // opp goal is in front of me
  const GoalModel::Goal& oppGoal = getLocalGoalModel().getOppGoal(getCompassDirection(), getFieldInfo());
  if(((oppGoal.leftPost+oppGoal.leftPost)*0.5).x > 0)
    getLocalGoalModel().opponentGoalIsValid = true;
  else
    getLocalGoalModel().ownGoalIsValid = true;

  if(getLocalGoalModel().opponentGoalIsValid)
  {
    getLocalGoalModel().frameWhenOpponentGoalWasSeen = getFrameInfo();
  }
  else
  {
    getLocalGoalModel().frameWhenOwnGoalWasSeen = getFrameInfo();
  }

  DEBUG_REQUEST("ActiveGoalLocator:draw_goal_model",
    FIELD_DRAWING_CONTEXT;
    if(getLocalGoalModel().opponentGoalIsValid)
      PEN("000000", 50);
    else
      PEN("FFFFFF", 50);

    CIRCLE(getLocalGoalModel().goal.leftPost.x, getLocalGoalModel().goal.leftPost.y, 50);
    CIRCLE(getLocalGoalModel().goal.rightPost.x, getLocalGoalModel().goal.rightPost.y, 50);
    LINE(getLocalGoalModel().goal.rightPost.x, getLocalGoalModel().goal.rightPost.y, getLocalGoalModel().goal.leftPost.x, getLocalGoalModel().goal.leftPost.y);
  );
  //
  /////////////////////////////////








  debugDrawings();
  debugPlots();
  debugStdOut();

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

    //std::cout << "Filter " << x << " is valid" <<  std::endl;

    for (unsigned int i = 0; i < ccSamples[x].sampleSet.size(); i++) {
      const AGLSample& sample = ccSamples[x].sampleSet[i];

        FIELD_DRAWING_CONTEXT;
        PEN(color, 30);
        CIRCLE(sample.translation.x, sample.translation.y, 20);
        TEXT_DRAWING(sample.translation.x+10,sample.translation.y+10, x);
    }
  });

  DEBUG_REQUEST("ActiveGoalLocator:draw_mean_of_each_valid_PF",

    for(unsigned int x = 0; x < 10; x++) {

      if (ccSamples[x].sampleSet.getIsValid()) {

        FIELD_DRAWING_CONTEXT;
        PEN("FF0000", 20);
        CIRCLE(ccSamples[x].sampleSet.mean.x, ccSamples[x].sampleSet.mean.y, 20);
      }
    }
  );

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

void ActiveGoalLocator::debugPlots() {

  for(unsigned int x = 0; x < 10; x++) {

      string id = convertIntToString(x);
      double totalWeighting = ccSamples[x].sampleSet.lastTotalWeighting;
      double averageWeighting = 0;
      if (ccSamples[x].sampleSet.size() > 0)
        averageWeighting = ccSamples[x].sampleSet.lastTotalWeighting/(double) ccSamples[x].sampleSet.size();

      PLOT("ActiveGoalLocator:lastAverageWeighting:"+id, averageWeighting);
      MODIFY("ActiveGoalLocator:lastAverageWeighting:"+id, averageWeighting);

      PLOT("ActiveGoalLocator:lastTotalWeighting:"+id, totalWeighting);
      MODIFY("ActiveGoalLocator:lastTotalWeighting:"+id, totalWeighting);
  }


} //end debugPlot

void ActiveGoalLocator::debugStdOut() {

    DEBUG_REQUEST("ActiveGoalLocator:which_filter_are_valid_to_StdOut",
      for(unsigned int i = 0; i < 10; i++) {
        if (ccSamples[i].sampleSet.getIsValid())
        std::cout << "Filter " << i << "is Valid" << std::endl;
      }
    );

}

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

  if (ccTrashBuffer.getLargestCluster().size() > 9) //FIXME: make param
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

  AGLSampleBuffer tmpSampleSetBuffer; //copy
  for (int i = 0; i < sampleSetBuffer.samples.getNumberOfEntries(); i++) {
      tmpSampleSetBuffer.samples.add(sampleSetBuffer[i]);
  }

  sampleSetBuffer.samples.clear(); //just clear and re-fill with unused entries

  //already known that sampleSet is empty!
  int n = 0;
  for (int i = 0; i < tmpSampleSetBuffer.samples.getNumberOfEntries(); i++) {

    //search all particles with ID of largest cluster and add them
    //TODO make n to param
    if (n < tmpSampleSetBuffer.samples.getNumberOfEntries() && n < (int)sampleSet.numberOfParticles  && tmpSampleSetBuffer[i].cluster == largestClusterID && largestClusterID > -1) {

      AGLSample tmpSample;
      tmpSample.color = tmpSampleSetBuffer[i].color;
      tmpSample.translation = tmpSampleSetBuffer[i].getPos();
      tmpSample.likelihood = 1.0/(double)sampleSet.size();
      sampleSet[n] = tmpSample;
      n++;

    } else { //if not used for filter, just copy
        sampleSetBuffer.samples.add(tmpSampleSetBuffer[i]);
    }
  }
  sampleSet.setValid();


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

  double totalWeighting = 0;

  for (unsigned int i = 0; i < sampleSet.size(); i++) {;

    double weighting(0.0);

        double diff = Math::normalize(sampleSet[i].getPos().angle() - post.position.angle()); //normalisieren, sonst pi--pi zu groß! value kann auch neg sein, da nur als norm in normalverteilung
        weighting = Math::gaussianProbability(diff, parameters.standardDeviationAngle);


    sampleSet[i].likelihood *= weighting;
    totalWeighting += sampleSet[i].likelihood;

  }//end for

  sampleSet.lastTotalWeighting = totalWeighting;

}//end updateByBallPercept

void ActiveGoalLocator::resampleGT07(AGLSampleSet& sampleSet, bool noise) {

  double totalWeighting = 0;

  for (unsigned int i = 0; i < sampleSet.size(); i++) {
    totalWeighting += sampleSet[i].likelihood;
  }//end for

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

//TOOLS

string ActiveGoalLocator::convertIntToString(int number)
{
   stringstream ss;
   ss << number;
   return ss.str();
}
