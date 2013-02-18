/**
 * @file ActiveGoalLocator.h
 *
 * @author <a href="mailto:scheunem@informatik.hu-berlin.de">Marcus Scheunemann</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * Implementation of class ActiveGoalLocator
 */

#include "ActiveGoalLocator.h"


// Debug
#include "Tools/Debug/DebugBufferedOutput.h"
#include "Tools/Debug/DebugModify.h"
#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"


// Math
#include "Tools/Math/Moments2.h"
#include "Tools/Math/Probabilistics.h"
#include "Tools/Math/Geometry.h"
#include <cmath>

#include "Representations/Modeling/GoalModel.h"

ActiveGoalLocator::ActiveGoalLocator()
  :
    ccSampleBuffer(parameters.thresholdCanopy)
{

  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_percept", "draw the current goal percept on field", false);
  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_percept_buffer", "draw the theSampleBuffer on field", true);
  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_hypotheses", "draw all hypotheses on field", true);
  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_mean_of_each_valid_PF", "", true);
  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_goal_model", "", false);

  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_sensor_association", "", false);

  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:which_filter_are_valid_to_StdOut", "Print the valid PFs in each time frame to check which is valid", false);

  postHypotheses.resize(10);

  for (unsigned int i = 0; i < postHypotheses.size(); i++) {
    postHypotheses[i].sampleSet.resetLikelihood();
  }

  /*
  CLUSTERING NOT USED YET
  for(unsigned int i = 0; i < ccSamples.size(); i++) 
    postHypotheses[i].canopyClustering.setClusterThreshold(parameters.thresholdCanopy);
  */
} //Constructor


void ActiveGoalLocator::execute()
{
  // don't update if the body state is not valid
  if (getBodyState().fall_down_state != BodyState::upright || // robot is not upright
      !(getBodyState().standByLeftFoot || getBodyState().standByRightFoot)) // no foot is on the ground
  {
    for (unsigned int i = 0; i < postHypotheses.size(); i++) {
      postHypotheses[i].sampleSet.setUnValid();
    }

    return;
  }

  // update the parameters of the hypotheses
  // (just in case they have been changed)
  for (unsigned int x = 0; x < postHypotheses.size(); x++) {
    postHypotheses[x].setParams(this->parameters.particleFilter);
  }


  // clear old percepts
  removeSamplesByFrameNumber(theSampleBuffer, 200); 


  //decrease lastTotalWeighting of each PF
  //TODO filter decreasen
  for (unsigned int x = 0; x < postHypotheses.size(); x++) 
  {
    PostHypothesis& hypothesis = postHypotheses[x];
    if (hypothesis.sampleSet.getIsValid()) 
    {
      hypothesis.sampleSet.lastTotalWeighting *= parameters.timeFilterRange;

      if (hypothesis.sampleSet.lastTotalWeighting < parameters.deletePFbyTotalWeightingThreshold) 
      {
        //hypothesis.sampleSet.setUnValid();
        std::cout << "delete filter '" << x << "' because " << postHypotheses[x].sampleSet.lastTotalWeighting << " < " << parameters.deletePFbyTotalWeightingThreshold << std::endl;
      }
     }
  }//end for

  //TODO reset likelihood of valid filter
  for (unsigned int x = 0; x < postHypotheses.size(); x++) 
  {
    PostHypothesis& hypothesis = postHypotheses[x];
    if (hypothesis.sampleSet.getIsValid()) {
      hypothesis.sampleSet.resetLikelihood();
    }
  }

  //////////////////////////
  // update by odometry
  //////////////////////////

  Pose2D odometryDelta = lastRobotOdometry - getOdometryData();
  //TODO introduce size of ccSamples
  for(unsigned int i = 0; i < postHypotheses.size(); i++) {
    postHypotheses[i].updateByOdometry(odometryDelta);
  }

  updateByOdometry(theSampleBuffer, odometryDelta);
  lastRobotOdometry = getOdometryData();



  //////////////////////////
  // update by percepts
  //////////////////////////

  // A ~ goal posts
  // B ~ hypotheses
  Assoziation sensorAssoziation(getGoalPercept().getNumberOfSeenPosts(), postHypotheses.size());

  // find the best association of the measurenets with the filters
  for (int i = 0; i < getGoalPercept().getNumberOfSeenPosts(); i++) 
  {
    for (unsigned int x = 0; x < postHypotheses.size(); x++) 
    {
      PostHypothesis& hypothesis = postHypotheses[x];
      if(hypothesis.sampleSet.getIsValid()) 
      {
        double confidence = hypothesis.getConfidenceForObservation(getGoalPercept().getPost(i));
        
        // store the best association for the measurement
        if(confidence > sensorAssoziation.getW4A(i)) {
          sensorAssoziation.addAssociation(i, x, confidence);
        }
      }
    }//end for
  }//end for


  DEBUG_REQUEST("ActiveGoalLocator:draw_sensor_association",
    FIELD_DRAWING_CONTEXT;
    PEN("FFFF00",10);
    for (int i = 0; i < getGoalPercept().getNumberOfSeenPosts(); i++) {
      const GoalPercept::GoalPost& post = getGoalPercept().getPost(i);
      int x = sensorAssoziation.getB4A(i); // get hypothesis for measurement
      if (x != -1) {
        LINE(post.position.x, post.position.y,
          postHypotheses[x].sampleSet.mean.x, postHypotheses[x].sampleSet.mean.y);
      }
    }
  );//DEBUG_REQUEST


  //Wie viele Pfosten moeglich???
  std::vector<GoalPercept::GoalPost> postArr (10);

  // update the hypotheses with the associated measurements
  for (int i = 0; i < getGoalPercept().getNumberOfSeenPosts(); i++) 
  {

    if (i < 10) { //Todo: param
      postArr[i]  = getGoalPercept().getPost(i);
    }

    const GoalPercept::GoalPost& post = getGoalPercept().getPost(i);
    int x = sensorAssoziation.getB4A(i); // get hypothesis for measurement
    if (x != -1)
    {
      postHypotheses[x].updateByGoalPostPercept(post);
    }
    else if (getGoalPercept().getPost(i).positionReliable) 
    {
      //no filter was updated
      //buffer just store reliable posts, because they are later clustered by position
      //insert the percept into the trashBuffer

      AGLBSample bufferSample;
      bufferSample.translation = post.position;
      bufferSample.color       = post.color;
      bufferSample.frameNumber = getFrameInfo().getFrameNumber();
      theSampleBuffer.add(bufferSample);
    }
  }//end for i < getGoalPercept().getNumberOfSeenPosts()


  // resample all PF
  for (unsigned int i = 0; i < postHypotheses.size(); i++) {
    postHypotheses[i].resampleGT07(postArr, true);
  }

  // calculate mean for all valid PFs
  for(unsigned int x = 0; x < postHypotheses.size(); x++) 
  {
    PostHypothesis& hypothesis = postHypotheses[x];
    Vector2<double> sum;

    if (hypothesis.sampleSet.getIsValid()) 
    {
      for (unsigned int i = 0; i < hypothesis.sampleSet.size(); i++) {
        sum += hypothesis.sampleSet[i].getPos();
      }

      hypothesis.sampleSet.mean = sum/(double)hypothesis.sampleSet.size();
    }
  }//end for x


  // check if there is a useable cluster in the SampleBuffer exists 
  // and create a new hypothesis if possible
  checkTrashBuffer(theSampleBuffer);


  //////////////////////////
  //   provide Goal Model
  // (extract the goal model)
  //////////////////////////
  estimateGoalModel();
  

  DEBUG_REQUEST("ActiveGoalLocator:draw_goal_model",
    FIELD_DRAWING_CONTEXT;
    if(getLocalGoalModel().opponentGoalIsValid) {
      PEN("000000", 50);
    } else {
      PEN("FFFFFF", 50);
    }

    CIRCLE(getLocalGoalModel().goal.leftPost.x, getLocalGoalModel().goal.leftPost.y, 50);
    CIRCLE(getLocalGoalModel().goal.rightPost.x, getLocalGoalModel().goal.rightPost.y, 50);
    LINE(getLocalGoalModel().goal.rightPost.x, getLocalGoalModel().goal.rightPost.y, getLocalGoalModel().goal.leftPost.x, getLocalGoalModel().goal.leftPost.y);
  );
  //
  /////////////////////////////////

  debugDrawings();
  debugPlots();

  DEBUG_REQUEST("ActiveGoalLocator:which_filter_are_valid_to_StdOut", debugStdOut(); );
}//end execute


void ActiveGoalLocator::estimateGoalModel()
{
  getLocalGoalModel().opponentGoalIsValid = false;
  getLocalGoalModel().ownGoalIsValid = false;
  getLocalGoalModel().someGoalWasSeen = false;

  //TODO Make as member
  unsigned int numOfValidFilter = 0;
  for(unsigned int i = 0; i < postHypotheses.size(); i++) 
  {
    if (postHypotheses[i].sampleSet.getIsValid()) {
      numOfValidFilter++;
    }
  }

  if (getGoalPercept().getNumberOfSeenPosts() > 0) 
  {
    getLocalGoalModel().someGoalWasSeen = true; //by post
    //frame Info when goal was seen not useful! New: some_goal_was seen
    getLocalGoalModel().goal.frameInfoWhenGoalLastSeen = getFrameInfo();
  }

  if (numOfValidFilter > 1) 
  {
    unsigned int id1 = 0;
    unsigned int id2 = 0;
    double lastDistError = 1000000;

    for(unsigned int x = 0; x < postHypotheses.size(); x++) 
    {
      PostHypothesis& hypothesisOne = postHypotheses[x];

      if (hypothesisOne.sampleSet.getIsValid()) 
      {
        double distError = 0;

        for(unsigned int i = x+1; i < postHypotheses.size(); i++) 
        {
          PostHypothesis& hypothesisTwo = postHypotheses[i];

          if (hypothesisTwo.sampleSet.getIsValid()) 
          {
            distError = abs((hypothesisOne.sampleSet.mean - hypothesisTwo.sampleSet.mean).abs() - getFieldInfo().goalWidth);

            //check if error becomes better
            if (distError < lastDistError) 
            { 
              lastDistError = distError;
              id1 = x;
              id2 = i;
            }
          }//end if
        }//end for i
      }//end if
    }//end for x

    //decide whether distError is feasable
    if (lastDistError < parameters.possibleGoalWidhtError) 
    {
      if (postHypotheses[id1].sampleSet.mean.angle() < postHypotheses[id2].sampleSet.mean.angle()) 
      {
        getLocalGoalModel().goal.leftPost  = postHypotheses[id1].sampleSet.mean;
        getLocalGoalModel().goal.rightPost = postHypotheses[id2].sampleSet.mean;
      } 
      else 
      {
        getLocalGoalModel().goal.leftPost  = postHypotheses[id2].sampleSet.mean;
        getLocalGoalModel().goal.rightPost = postHypotheses[id1].sampleSet.mean;
      }
    }//end if

  }//end if numOfValidFilter

  // opp goal is in front of me
  const GoalModel::Goal& oppGoal = getLocalGoalModel().getOppGoal(getCompassDirection(), getFieldInfo());
  if(((oppGoal.leftPost+oppGoal.leftPost)*0.5).x > 0) {
    getLocalGoalModel().opponentGoalIsValid = true;
  } else {
    getLocalGoalModel().ownGoalIsValid = true;
  }

  if(getLocalGoalModel().opponentGoalIsValid) {
    getLocalGoalModel().frameWhenOpponentGoalWasSeen = getFrameInfo();
  } else {
    getLocalGoalModel().frameWhenOwnGoalWasSeen = getFrameInfo();
  }
}//end estimateGoalModel


void ActiveGoalLocator::debugDrawings() 
{
  DEBUG_REQUEST("ActiveGoalLocator:draw_goal_model",
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 50);
    CIRCLE(getLocalGoalModel().goal.leftPost.x, getLocalGoalModel().goal.leftPost.y, 50);
    PEN("0000FF", 50);
    CIRCLE(getLocalGoalModel().goal.rightPost.x, getLocalGoalModel().goal.rightPost.y, 50);
    LINE(getLocalGoalModel().goal.rightPost.x, getLocalGoalModel().goal.rightPost.y, getLocalGoalModel().goal.leftPost.x, getLocalGoalModel().goal.leftPost.y);
  );

  DEBUG_REQUEST("ActiveGoalLocator:draw_hypotheses",
    FIELD_DRAWING_CONTEXT;
    for (unsigned int x = 0; x < postHypotheses.size(); x++ ) 
    {
      string color = ColorClasses::colorClassToHex((ColorClasses::Color)((x+3)%ColorClasses::numOfColors));
      postHypotheses[x].drawParticles(color, x);
    }//end for
  );


  DEBUG_REQUEST("ActiveGoalLocator:draw_mean_of_each_valid_PF",
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 20);
    for(unsigned int x = 0; x < postHypotheses.size(); x++) 
    {
      if (postHypotheses[x].sampleSet.getIsValid()) {
        CIRCLE(postHypotheses[x].sampleSet.mean.x, postHypotheses[x].sampleSet.mean.y, 20);
      }
    }
  );

  DEBUG_REQUEST("ActiveGoalLocator:draw_percept",
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 30);
    for (int i = 0; i < getGoalPercept().getNumberOfSeenPosts(); i++) 
    {
      const Vector2<double>& percept = getGoalPercept().getPost(i).position;
      CIRCLE(percept.x, percept.y, 20);
    }//end for
  );


  DEBUG_REQUEST("ActiveGoalLocator:draw_percept_buffer",
    //std::cout << "size" << theSampleBuffer.size() << std::endl;
    FIELD_DRAWING_CONTEXT;
    PEN("000000", 50);
    for (unsigned int i = 0; i < theSampleBuffer.size(); i++) 
    {
      const AGLBSample& sample = theSampleBuffer[i];
      CIRCLE(sample.translation.x, sample.translation.y, 20);
      //std::cout << "it" << i << "x" << sample.translation.x << "y" << sample.translation.y << std::endl;
    }//end for
  );

} //end debugDrawings

void ActiveGoalLocator::debugPlots() 
{
  for(unsigned int x = 0; x < postHypotheses.size(); x++) 
  {
    string id = convertIntToString(x);
    double totalWeighting = postHypotheses[x].sampleSet.lastTotalWeighting;
    double averageWeighting = 0;
    if (postHypotheses[x].sampleSet.size() > 0) {
      averageWeighting = postHypotheses[x].sampleSet.lastTotalWeighting/(double) postHypotheses[x].sampleSet.size();
    }

    PLOT("ActiveGoalLocator:lastAverageWeighting:"+id, averageWeighting);
    //MODIFY("ActiveGoalLocator:lastAverageWeighting:"+id, averageWeighting);

    PLOT("ActiveGoalLocator:lastTotalWeighting:"+id, totalWeighting);
    //MODIFY("ActiveGoalLocator:lastTotalWeighting:"+id, totalWeighting);
  }//end for

} //end debugPlot

void ActiveGoalLocator::debugStdOut() 
{
  for(unsigned int i = 0; i < postHypotheses.size(); i++) {
    if (postHypotheses[i].sampleSet.getIsValid()) {
      DOUT( "Filter '" << i << "' is Valid      " << std::endl );
    }
  }
}//end debugStdOut

void ActiveGoalLocator::updateByOdometry(AGLSampleBuffer& sampleSet, const Pose2D& odometryDelta) const
{
  for (unsigned int i = 0; i < sampleSet.size(); i++) 
  {
    // move each particle with odometry
    sampleSet[i].translation = odometryDelta * sampleSet[i].translation;
  }
}//end updateByOdometry


//TODO side effect with ccSamples los werdn
void ActiveGoalLocator::checkTrashBuffer(AGLSampleBuffer& sampleBuffer)
{
  ccSampleBuffer.cluster(theSampleBuffer);
  if (ccSampleBuffer.size() > 0 && ccSampleBuffer.getLargestCluster().size() > 9) //FIXME: make param
  {
    for (unsigned int i = 0; i < postHypotheses.size(); i++) 
    {
      // find an invalide hypothesis and initialize a new filter at its place
      if (!postHypotheses[i].sampleSet.getIsValid()) 
      {
        initFilterByBuffer(ccSampleBuffer.getLargestClusterID(), sampleBuffer, postHypotheses[i].sampleSet);
        postHypotheses[i].setParams(this->parameters.particleFilter);
        break; //xxx
      }  //end if ccSamples[i] is empty
    }
  }//if largestCluster > 0
}// checkTrashBuffer


void ActiveGoalLocator::initFilterByBuffer(const int& largestClusterID, AGLSampleBuffer& sampleSetBuffer, AGLSampleSet& sampleSet)
{
  if(largestClusterID < 0) return;

  AGLSampleBuffer tmpSampleSetBuffer; //copy
  for (int i = 0; i < sampleSetBuffer.getNumberOfEntries(); i++) {
    tmpSampleSetBuffer.add(sampleSetBuffer[i]);
  }

  sampleSetBuffer.clear(); //just clear and re-fill with unused entries

  //already known that sampleSet is empty!
  int n = 0;
  Vector2<double> sum;
  for (int i = 0; i < tmpSampleSetBuffer.getNumberOfEntries(); i++) 
  {
    //search all particles with ID of largest cluster and add them
    //TODO make n to param
    if (n < tmpSampleSetBuffer.getNumberOfEntries() && n < (int)sampleSet.size() &&
        tmpSampleSetBuffer[i].cluster == largestClusterID) 
    {
      AGLSample tmpSample;
      tmpSample.color = tmpSampleSetBuffer[i].color;
      tmpSample.translation = tmpSampleSetBuffer[i].getPos();
      tmpSample.likelihood = 1.0/(double)sampleSet.size();
      sampleSet[n] = tmpSample;
      n++;

      sum += tmpSample.translation;
    } 
    else //if not used for filter, just copy
    { 
      sampleSetBuffer.add(tmpSampleSetBuffer[i]);
    }
  }//end for

  ASSERT((int)sampleSet.size() == n);

  // update the mean of the filter and set it valid
  sampleSet.mean = sum / (double)sampleSet.size();
  sampleSet.setValid();
}//copyFromBufferToFilter


void ActiveGoalLocator::removeSamplesByFrameNumber(AGLSampleBuffer& sampleSet, const unsigned int maxFrames) const
{
  if( sampleSet.getNumberOfEntries() > 0 &&
      getFrameInfo().getFrameNumber() - sampleSet.first().frameNumber > maxFrames)
  {
    sampleSet.removeFirst();
  }
}//end removeSamplesByFrameNumber


//TOOLS

string ActiveGoalLocator::convertIntToString(int number)
{
   stringstream ss;
   ss << number;
   return ss.str();
}//end convertIntToString
