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

ActiveGoalLocator::ActiveGoalLocator() {

  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_samples", "draw the sample set on field", false);
  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_goalCenter", "draw the center of goal on field", false);
  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_percept", "draw the goal percept on field", false);

  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_major_of_cluster", "", false);
  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_minor_of_cluster", "", false);

  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_goal_model", "", false);

  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:draw_GoalLine_via_moments", "", false);

  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:with_distance_gaussian", "", true);
  DEBUG_REQUEST_REGISTER("ActiveGoalLocator:cluster_centers", "", false);

  goalWidth = (getFieldInfo().opponentGoalPostLeft - getFieldInfo().opponentGoalPostRight).abs();

  timeFilter = 0.000001;

  /*for (unsigned int i = 0; i < sampleSet.size(); i++) {
    sampleSet[i].likelihood = 0.01;
  }*/

} //Constructor

void ActiveGoalLocator::execute() {

  // reset
  getLocalGoalModel().someGoalWasSeen = false;
  getLocalGoalModel().opponentGoalIsValid = false;
  getLocalGoalModel().ownGoalIsValid = false;


  /* 18.02.2012
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



  /* 18.02.2012
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


  // don't update if the body state is not valid
  if (getBodyState().fall_down_state != BodyState::upright || // robot is not upright
    !(getBodyState().standByLeftFoot || getBodyState().standByRightFoot) // no foot is on the ground
    ) {
    sampleSet.clear();
  }

  timeFilter += parameters.timeFilterRange * averageWeighting;

  //TODO: Recognize which Goal was seen
  for (int i = 0; i < getGoalPercept().getNumberOfSeenPosts(); i++) { //&& timeFilter < parameters.sigmaWeightingThreshhold
    //should be more flexible. when two posts are seen, and just one is reliable -> also add possible!
    if (getGoalPercept().getPost(i).positionReliable) {
      if (sampleSet.size() < 40) {
        Sample s;
        s.position = getGoalPercept().getPost(i).position;
        sampleSet.push_back(s);
      } else {
        int idx = Math::random(40);
        sampleSet[idx].position = getGoalPercept().getPost(i).position;
      }
    }
  }

  for (unsigned int i = 0; i < sampleSet.size(); i++) {
    sampleSet[i].likelihood = 0.1;
  }

  updateByRobotOdometry();
  updateByGoalPercept();
  resampleGT07(true);

  cluster();

 double dNumOfCluster = (double) numOfClusters;
    MODIFY("AGLSP:numOfCluster", dNumOfCluster);

  //For Modelling

  /* 18.02.2012
  //should be decided in updateByGoalPost!

  bool oppGoalSeen = false;
  bool ownGoalSeen = false;

  ColorClasses::Color opponentGoalColor = ColorClasses::yellow;

  if (getPlayerInfo().gameData.teamColor == GameData::red) {
    opponentGoalColor = ColorClasses::skyblue;
  }
  if (lastGoalColor == opponentGoalColor) {
    oppGoalSeen = true;
  } else {
    ownGoalSeen = true;
  }*/

  //get posts by cluster
  if (numOfClusters != 0) {

      //initialize with first cluster
      /********************************************************/
      //locate the two biggest (size) clusters!
      //not possible yet! mostly two big clusters behinds each other!!!
      /*
      Vector2<double> leftPost, rightPost;
      int idxOfFirCluster = 0;
      int idxOfSecCluster = 0;

      leftPost = clusters[0].center.position;
      rightPost = clusters[0].center.position;


      for (int i = 0; i < numOfClusters; i++) {
        if (clusters[idxOfFirCluster].size > clusters[i].size) {
          idxOfFirCluster = i;
          idxOfSecCluster = i;
        }
      }
      for (int i = 0; i < numOfClusters; i++) {
        if (clusters[idxOfFirCluster].size < clusters[i].size) {
          idxOfFirCluster = i;
        }
      }
      for (int i = 0; i < numOfClusters; i++) {
        if (clusters[idxOfSecCluster].size < clusters[i].size && i != idxOfFirCluster) {
          idxOfSecCluster = i;
        }
      }

      if(clusters[idxOfFirCluster].center.position.angle() > clusters[idxOfSecCluster].center.position.angle()) {
        leftPost  = clusters[idxOfFirCluster].center.position;
        rightPost = clusters[idxOfSecCluster].center.position;
      } else {
        leftPost  = clusters[idxOfSecCluster].center.position;
        rightPost = clusters[idxOfFirCluster].center.position;
      }
      */
     /********************************************************/

      //locate clusters the most left and right one!
      //not possible yet! mostly two big clusters behinds each other!!!

      Vector2<double> leftPost, rightPost;
      leftPost = clusters[0].center.position;
      rightPost = clusters[0].center.position;

      for (int i = 0; i < numOfClusters; i++) {
        if (leftPost.angle() < clusters[i].center.position.angle()) {
          leftPost = clusters[i].center.position;
        }
      }

      for (int i = 0; i < numOfClusters; i++) {
        if (rightPost.angle() > clusters[i].center.position.angle()) {
          rightPost = clusters[i].center.position;
        }
      }

      DEBUG_REQUEST("ActiveGoalLocator:cluster_centers",
        FIELD_DRAWING_CONTEXT;
        PEN("FF0000", 50);
        CIRCLE(leftPost.x, leftPost.y, 50);
        TEXT_DRAWING(leftPost.x+10,leftPost.y+10, "L");
        PEN("0000FF", 50);
        CIRCLE(rightPost.x, rightPost.y, 50);
        TEXT_DRAWING(rightPost.x+10,rightPost.y+10, "R");
      );

      /*18.02.2012
        tore malen welche lokal vorhanden sind

        if (lastGoalColor == ColorClasses::skyblue) {
        getLocalGoalModel().blueGoal.leftPost = leftPost;
        getLocalGoalModel().blueGoal.rightPost = rightPost;
        // TODO: create a separate model for the another goal
        getLocalGoalModel().calculateYellowByBlue(getFieldInfo().xLength);
        getLocalGoalModel().blueGoal.frameInfoWhenGoalLastSeen = getFrameInfo();
      } else if(lastGoalColor == ColorClasses::yellow) {
        getLocalGoalModel().yellowGoal.leftPost = leftPost;
        getLocalGoalModel().yellowGoal.rightPost = rightPost;
        // TODO: create a separate model for the another goal
        getLocalGoalModel().calculateBlueByYellow(getFieldInfo().xLength);
        getLocalGoalModel().yellowGoal.frameInfoWhenGoalLastSeen = getFrameInfo();
      }
      */

      getLocalGoalModel().goal.leftPost  = leftPost;
      getLocalGoalModel().goal.rightPost = rightPost;
      //frame Info when goal was seen not useful! New: some_goal_was seen
      getLocalGoalModel().goal.frameInfoWhenGoalLastSeen = getFrameInfo();

    //caculated by right and left post!
    Vector2<double> goalCenter;
    goalCenter = (leftPost - rightPost) * 0.5 + rightPost;

    /*18.02.2012 same colors ... no decision of own ord opp goal
    if (ownGoalSeen) getLocalGoalModel().frameWhenOwnGoalWasSeen = getFrameInfo();
    if (oppGoalSeen) {
      //getLocalGoalModel().frameWhenOpponentGoalWasSeen = getFrameInfo();
      getLocalGoalModel().seen_center = goalCenter; //opponentGoal Position
      getLocalGoalModel().seen_angle = goalCenter.angle(); //opponentGoal Angle
    }*/

    //TODO check decision for opp goal!
    //getOppGoal == goal ...
    if (getCompassDirection().angle > Math::pi_2) {

        getLocalGoalModel().seen_center = goalCenter;
        getLocalGoalModel().seen_angle = goalCenter.angle();
    }

    DEBUG_REQUEST("ActiveGoalLocator:draw_goalCenter",
      FIELD_DRAWING_CONTEXT;
      PEN("FF0000", 50);
      CIRCLE(goalCenter.x, goalCenter.y, 50);
    );

  } else { //numOfClusters != 0
    //do nothing -> symbol look for goal!
  }

  // say it Model is Valid when more than one cluster is seen
  //commented 18.02.2012
  //getLocalGoalModel().opponentGoalIsValid = (oppGoalSeen && numOfClusters > 1);
  //getLocalGoalModel().ownGoalIsValid = (ownGoalSeen && numOfClusters > 1);

  debugDrawings();

  STOPWATCH_STOP("ActiveGoalLocator");

}//end execute

void ActiveGoalLocator::debugDrawings() {

  /*18.02.2012
    commented because of same colored goals
  DEBUG_REQUEST("ActiveGoalLocator:draw_goal_model",

  if (lastGoalColor == ColorClasses::skyblue) {

    FIELD_DRAWING_CONTEXT;
      PEN("FF0000", 50);
      CIRCLE(getLocalGoalModel().blueGoal.leftPost.x, getLocalGoalModel().blueGoal.leftPost.y, 50);

      PEN("0000FF", 50);
      CIRCLE(getLocalGoalModel().blueGoal.rightPost.x, getLocalGoalModel().blueGoal.rightPost.y, 50);
      LINE(getLocalGoalModel().blueGoal.rightPost.x, getLocalGoalModel().blueGoal.rightPost.y, getLocalGoalModel().blueGoal.leftPost.x, getLocalGoalModel().blueGoal.leftPost.y);
  } else if (lastGoalColor == ColorClasses::yellow) {
    FIELD_DRAWING_CONTEXT;
      PEN("FF0000", 50);
      CIRCLE(getLocalGoalModel().yellowGoal.leftPost.x, getLocalGoalModel().yellowGoal.leftPost.y, 50);
      PEN("FFFF00", 50);
      CIRCLE(getLocalGoalModel().yellowGoal.rightPost.x, getLocalGoalModel().yellowGoal.rightPost.y, 50);
      LINE(getLocalGoalModel().yellowGoal.rightPost.x, getLocalGoalModel().yellowGoal.rightPost.y, getLocalGoalModel().yellowGoal.leftPost.x, getLocalGoalModel().yellowGoal.leftPost.y);
  }

);*/

    DEBUG_REQUEST("ActiveGoalLocator:draw_goal_model",

      FIELD_DRAWING_CONTEXT;
        PEN("FF0000", 50);
        CIRCLE(getLocalGoalModel().goal.leftPost.x, getLocalGoalModel().goal.leftPost.y, 50);
        PEN("0000FF", 50);
        CIRCLE(getLocalGoalModel().goal.rightPost.x, getLocalGoalModel().goal.rightPost.y, 50);
        LINE(getLocalGoalModel().goal.rightPost.x, getLocalGoalModel().goal.rightPost.y, getLocalGoalModel().goal.leftPost.x, getLocalGoalModel().goal.leftPost.y);
  );

  DEBUG_REQUEST("ActiveGoalLocator:draw_samples",
  for (unsigned int i = 0; i < sampleSet.size(); i++) {
    const Sample& sample = sampleSet[i];

    string color = ColorClasses::colorClassToHex((ColorClasses::Color)((sample.cluster+2)%ColorClasses::numOfColors));

      FIELD_DRAWING_CONTEXT;
      PEN(color, 30);
      CIRCLE(sample.position.x, sample.position.y, 20);
      TEXT_DRAWING(sample.position.x+10,sample.position.y+10,sample.cluster);

  });

  DEBUG_REQUEST("ActiveGoalLocator:draw_percept",
  for (int i = 0; i < getGoalPercept().getNumberOfSeenPosts(); i++) {
    const Vector2<double> percept = getGoalPercept().getPost(i).position;

      FIELD_DRAWING_CONTEXT;
      PEN("FF0000", 30);
      CIRCLE(percept.x, percept.y, 20);

  });

} //end debugDrawings

void ActiveGoalLocator::updateByRobotOdometry() {

  // negative odometry
  Pose2D odometryDelta = lastRobotOdometry - getOdometryData();
  lastRobotOdometry = getOdometryData();

  for (unsigned int i = 0; i < sampleSet.size(); i++) {
    sampleSet[i].position = odometryDelta * sampleSet[i].position;
  }

}//end updateByRobotOdometry

void ActiveGoalLocator::updateByGoalPercept() {

  if (!getGoalPercept().getNumberOfSeenPosts()) return;

  bool oppGoalSeen = false;
  bool ownGoalSeen = false;

  /* 18.02.2012
  ColorClasses::Color opponentGoalColor = ColorClasses::yellow;
  if (getPlayerInfo().gameData.teamColor == GameData::red) {
    opponentGoalColor = ColorClasses::skyblue;
  }*/

  for (unsigned int i = 0; i < sampleSet.size(); i++) {
    Sample& sample = sampleSet[i];

    //double value/*distance*/ = (sample.position - getGoalPercept().getPost(0).position).abs();

    //check if particle "close" to sample in relation to its angle
    //double angle = getGoalPercept().getPost(0).position.angle();
    //double weighting = Math::gaussianProbability(angle, parameters.standardDeviationAngle);

    double weighting(0.0);

    for (int i = 0; i < getGoalPercept().getNumberOfSeenPosts(); i++) {

      if (getGoalPercept().getPost(i).positionReliable) {
        //check with distance to perception
        double value = Math::normalize(sample.position.angle() - getGoalPercept().getPost(i).position.angle()); //normalisieren, sonst pi--pi zu groß! value kann auch neg sein, da nur als norm in normalverteilung
        weighting = Math::gaussianProbability(value, parameters.standardDeviationAngle);

        //Problem: winkelfehler und Distanz gemischt -> nicht vergleichbare fehler!

        double diff = (sample.position - getGoalPercept().getPost(i).position).abs();
        MODIFY("ActiveGoalLocator:dist_diff_of_sample_and_percept", diff);
        MODIFY("ActiveGoalLocator:goalWidth", goalWidth);

        double value2 = abs((sample.position - getGoalPercept().getPost(i).position).abs() - goalWidth);

        DEBUG_REQUEST("ActiveGoalLocator:with_distance_gaussian",
          weighting += Math::gaussianProbability(value2, parameters.standardDeviationDist)*670.0;
        );
      } 

    //TODO check this decision
    //18.02.2012
    if (abs(getCompassDirection().angle) > Math::pi_2) {
        oppGoalSeen = true;
      } else {
        ownGoalSeen = true;
      }
    }

    sample.likelihood *= weighting;
  }//end for


  if (ownGoalSeen) getLocalGoalModel().frameWhenOwnGoalWasSeen = getFrameInfo();
  if (oppGoalSeen) {
    getLocalGoalModel().frameWhenOpponentGoalWasSeen = getFrameInfo();
  }
}//end updateByBallPercept

void ActiveGoalLocator::resampleGT07(bool noise) {

  double totalWeighting = 0;

  //Should also be calculated while "inserting"? -> NO ... better independent from update or insert!
  for (unsigned int i = 0; i < sampleSet.size(); i++) {
    totalWeighting += sampleSet[i].likelihood;
  }//end for

  MODIFY("ActiveGoalLocator:totalWeighting", totalWeighting); //justDEBUG ERROR!!

  if (!sampleSet.empty())
    averageWeighting = totalWeighting / (double) sampleSet.size();
  else averageWeighting = 0.0001234;

  //PLOT("ActiveGoalLocator:averageWeighting", averageWeighting);
  MODIFY("ActiveGoalLocator:averageWeighting", averageWeighting); //justDEBUG

  // copy the samples
  // TODO: use memcopy?
  std::vector<Sample> oldSampleSet = sampleSet;

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
        sampleSet[n].position.x += (Math::random() - 0.5) * parameters.processNoiseDistance;
        sampleSet[n].position.y += (Math::random() - 0.5) * parameters.processNoiseDistance;
      }

      n++;
      count++;

    }//end while
  }//end for

}//end resampleGT07


//////////////////////////////
// Outsourding Cluster!
//////////////////////////////

void ActiveGoalLocator::cluster()
{

  //vector<CanopyCluster> clusters;
  numOfClusters = 0; //not initialiszed here anymore, used in top

  for (unsigned int j = 0; j < sampleSet.size(); j++)
  {
    sampleSet[j].cluster = -1; // no cluster

    // look for a cluster with the smallest distance
    double minDistance = 10000; // 10m
    int minIdx = -1;

    for (int k = 0; k < numOfClusters; k++) { //FIXME, static number
      double dist = clusters[k].distance(sampleSet[j]);
      if(dist < minDistance)
      {
        minIdx = k;
        minDistance = dist;
      }
    }//end for

    // try to add to the nearedst cluster
    if(minIdx != -1 && isInCluster(clusters[minIdx], sampleSet[j]))
    {
      sampleSet[j].cluster = minIdx;
      clusters[minIdx].add(sampleSet[j]);
    }
    // othervise create new cluster
    else if(numOfClusters < maxNumberOfClusters)
    {
      // initialize a new cluster
      clusters[numOfClusters].center = sampleSet[j];
      clusters[numOfClusters].clusterSum = sampleSet[j].position;
      clusters[numOfClusters].size = 1;
      sampleSet[j].cluster = numOfClusters;
      numOfClusters++;
    }//end if


  }//end for

  // merge close clusters
  for(int k=0; k< numOfClusters; k++)
  {
    if(clusters[k].size < 4) {
      continue;
    }
    for(int j=k+1;j<numOfClusters;j++) {
      if ( clusters[j].size < 4) {
        continue;
      }
      // merge the clusters k and j
      if((clusters[k].center.position - clusters[j].center.position).abs() < 500)
      {
        clusters[k].size += clusters[j].size;
        clusters[j].size = 0;
        //this is kind of pointless because we wont use the new center afterwards.
        //the merge will be more accurate, but it is not that important because we only
        //merge close clusters, so the error is small
        //remove in case of performance issues:
        clusters[k].center.position = (clusters[k].center.position + clusters[j].center.position) * 0.5;
        for (unsigned int i = 0; i < sampleSet.size(); i++)
        {
          if(sampleSet[i].cluster == j) {
              sampleSet[i].cluster = k;
          }
        } //end for i
      } //end if abs < 500
    } // end for j
  } //end for k
}//end cluster


int ActiveGoalLocator::getClusterSize(const Vector2<double> start)
{
  CanopyCluster cluster;
  cluster.center.position = start;
  cluster.clusterSum = start;
  cluster.size = 1;

  for (unsigned int j = 0; j < sampleSet.size(); j++)
  {
    sampleSet[j].cluster = -1;
    if(isInCluster(cluster, sampleSet[j]))
    {
      sampleSet[j].cluster = 0;
      cluster.add(sampleSet[j]);
    }
  }//end for j

  return (int)cluster.size;
}//end getClusteSize

bool ActiveGoalLocator::isInCluster(const CanopyCluster& cluster, const Sample& sample) const
{
  return cluster.distance(sample) < parameters.thresholdCanopy;
}//end isInCluster


void ActiveGoalLocator::CanopyCluster::add(const Sample& sample)
{
  size++;
  clusterSum += sample.position;
  center.position = (clusterSum / size);
}//end add

// TODO: make it switchable
double ActiveGoalLocator::CanopyCluster::distance(const Sample& sample) const
{
  return euclideanDistance(sample);
  //return manhattanDistance(sample);
}//end distance

double ActiveGoalLocator::CanopyCluster::manhattanDistance(const Sample& sample) const
{
  return std::fabs(center.position.x - sample.position.x)
       + std::fabs(center.position.y - sample.position.y);
}//end manhattanDistance

double ActiveGoalLocator::CanopyCluster::euclideanDistance(const Sample& sample) const
{
  return (center.position - sample.position).abs();
}//end euclideanDistance
