/**
* @file MonteCarloSelfLocator.cpp
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* Implementation of class MonteCarloSelfLocator
*/

#include "MonteCarloSelfLocator.h"

// debug
#include "Tools/Debug/Stopwatch.h"
#include "Tools/Debug/DebugBufferedOutput.h"

// tools
#include "Tools/Math/Probabilistics.h"

using namespace std;

MonteCarloSelfLocator::MonteCarloSelfLocator() 
  :
    //gridClustering(sampleSet),
    canopyClustering(theSampleSet, parameters.thresholdCanopy),
    initialized(false),
    // ...whole field by default
    fieldMin(-getFieldInfo().xFieldLength/2.0, -getFieldInfo().yFieldLength/2.0),
    fieldMax( getFieldInfo().xFieldLength/2.0,  getFieldInfo().yFieldLength/2.0)
{

  resetSampleSet(theSampleSet);

  // infrastructure
  DEBUG_REQUEST_REGISTER("MCSL:draw_field_lines", "draws the field lines", false);
  DEBUG_REQUEST_REGISTER("MCSL:draw_corner_distances:L", "draws distances to the nearest corner for all points", false);
  DEBUG_REQUEST_REGISTER("MCSL:draw_corner_distances:TX", "draws distances to the nearest corner for all points", false);

  DEBUG_REQUEST_REGISTER("MCSL:draw_visible_lines", "draw the estimated line which is next to the robot pose", false);   
  DEBUG_REQUEST_REGISTER("MCSL:draw_sensor_belief", "", false);

  DEBUG_REQUEST_REGISTER("MCSL:draw_distances:on", "...", false);
  DEBUG_REQUEST_REGISTER("MCSL:draw_distances:all", "draws distances to the nearest line for all points", false);
  DEBUG_REQUEST_REGISTER("MCSL:draw_distances:along", "draws distances to the nearest line for all points", false);
  DEBUG_REQUEST_REGISTER("MCSL:draw_distances:across", "draws distances to the nearest line for all points", false);
  DEBUG_REQUEST_REGISTER("MCSL:draw_distances:long", "draws distances to the nearest line for all points", false);
  DEBUG_REQUEST_REGISTER("MCSL:draw_distances:short", "draws distances to the nearest line for all points", false);
  DEBUG_REQUEST_REGISTER("MCSL:draw_distances:circle", "draws distances to the nearest line for all points", false);

  // debug
  DEBUG_REQUEST_REGISTER("MCSL:reset_samples", "reset the sample set", false);

  // percepts
  DEBUG_REQUEST_REGISTER("MCSL:draw_seen_goal_posts", "draws the seen goal posts", false);


  // experimental parts of the algorithm
  DEBUG_REQUEST_REGISTER("MCSL:use_lines_constraints", "Update particles by constraints", false);
  DEBUG_REQUEST_REGISTER("MCSL:draw_corner_votes", "marks the corners on the field the particles are voting for", false);
  
  //see Find Largest Cluster: commented out
  //DEBUG_REQUEST_REGISTER("MCSL:draw_best_cells","draws best cell (self locator)", false);
  //DEBUG_REQUEST_REGISTER("MCSL:draw_best_cells_shifted","draws best cell in the shifted grid (self locator)", false);

   
  // sample plots
  DEBUG_REQUEST_REGISTER("MCSL:draw_Samples", "draw sample set before resampling", false);
  DEBUG_REQUEST_REGISTER("MCSL:draw_Cluster", "draw the clustered particle set", false);
  DEBUG_REQUEST_REGISTER("MCSL:draw_deviation", "draw the ", false);
  
  
  // resulting position
  DEBUG_REQUEST_REGISTER("MCSL:draw_position","draw robots position (self locator)", false);

  // draw estimated goal model
  DEBUG_REQUEST_REGISTER("MCSL:draw_SelfLocGoalModel", "draw the goal model based on self locator", false);


  DEBUG_REQUEST_REGISTER("MCSL:plots", "enables the debug plots", false);
}


void MonteCarloSelfLocator::resample(SampleSet& sampleSet)
{
  // normalize
  sampleSet.normalize();

  SampleSet newSampleSet = sampleSet;

  //idea take all particles which fit the equidistant array

  double aggr = 0;
  int i = 0;//NUM_OF_PARTICLES-1; //particle number to copy
  double offset = 1.0/(2*sampleSet.numberOfParticles);

  for (unsigned int j = 0; j < sampleSet.numberOfParticles; j++)   //j is the value of the equidistant grid
  {
    //take which particle?

    while ((aggr + sampleSet[i].likelihood) <= (double)(j)/sampleSet.numberOfParticles + offset) 
    {
      aggr += sampleSet[i].likelihood;
      i++;
    }
    newSampleSet[j].translation.x = sampleSet[i].translation.x + (Math::random()-0.5)*125;
    newSampleSet[j].translation.y = sampleSet[i].translation.y + (Math::random()-0.5)*125;
    newSampleSet[j].rotation = Math::normalize(sampleSet[i].rotation + (Math::random()-0.5)*0.1);
  }//end for

  int numberOfPartiklesToResample = (int)(((double)sampleSet.numberOfParticles)*0.1+0.5);

  for (unsigned int j = 0; j < sampleSet.numberOfParticles-numberOfPartiklesToResample; j++)
  {
    int index = (int)(Math::random()*(sampleSet.numberOfParticles-1)+0.5);
    sampleSet[sampleSet.numberOfParticles-1-j] = newSampleSet[index];
  }//end for

  //sampleSet.sort();


  //reset 2 new samples
  for (int j = 0; j < numberOfPartiklesToResample; j++)
  {
    //int idx = (int)(Math::random()*NUM_OF_PARTICLES);
    sampleSet[j].translation.x = (Math::random()*2-1)*getFieldInfo().xPosOpponentGoal;
    sampleSet[j].translation.y = (Math::random()*2-1)*getFieldInfo().yPosLeftSideline;
    sampleSet[j].rotation = (Math::random()*2-1)*Math::pi;
  }//end for
  
}//end resample


void MonteCarloSelfLocator::updateByOldPose(SampleSet& sampleSet) const
{     
  double sigmaDistance = parameters.sigmaDistanceOwnPose;
  double sigmaAngle = parameters.sigmaAngleOwnPose;

  updateByPose(sampleSet, getRobotPose(), sigmaDistance, sigmaAngle);
}//end updateByOldPose


void MonteCarloSelfLocator::updateByGoalModel(SampleSet& sampleSet) const
{
  double sigmaDistance = parameters.sigmaDistanceGoalModel;
  double sigmaAngle = parameters.sigmaAngleGoalModel;

  Pose2D pose = getSensingGoalModel().calculatePose(getCompassDirection(), getFieldInfo());

  updateByPose(sampleSet, pose, sigmaDistance, sigmaAngle);
}//end updateByGoalModel


void MonteCarloSelfLocator::updateByPose(SampleSet& sampleSet, Pose2D pose, double sigmaDistance, double sigmaAngle) const
{
  const double bestPossibleDistanceWeighting = 1.0;
  //const double bestPossibleAngleWeighting = 1.0;

  for (unsigned int j = 0; j < sampleSet.numberOfParticles; j++)
  {   
    double distDif = (sampleSet[j].translation - pose.translation).abs();
    //sampleSet[j].likelihood *= computeAngleWeighting(pose.rotation, sampleSet[j].rotation, sigmaAngle, bestPossibleAngleWeighting);
    sampleSet[j].likelihood *= Math::gaussianProbability(distDif,sigmaDistance) / bestPossibleDistanceWeighting; 
  } //end for j
}//end updateByPose


void MonteCarloSelfLocator::updateByGoalPosts(SampleSet& sampleSet) const
{
  const double sigmaAngle    = parameters.sigmaAngleGoalPost;
  const double sigmaDistance = parameters.sigmaDistanceGoalPost;
  const double cameraHeight  = getCameraMatrix().translation.z;

  
  //with the GoalPercept
  for(int i = 0; i < getGoalPercept().getNumberOfSeenPosts(); i++)
  {
    const GoalPercept::GoalPost& seenPost = getGoalPercept().getPost(i);

    const double seenDistance = seenPost.position.abs();
    const double seenAngle = seenPost.position.angle();

    // classify the seen goal

    const Vector2<double>* leftGoalPosition;
    const Vector2<double>* rightGoalPosition;
    
    if( getFieldSidePercept().facedFieldSide == FieldSidePercept::opponent ||
        (getFieldSidePercept().facedFieldSide == FieldSidePercept::unknown && 
         fabs(Math::normalize(getRobotPose().rotation + seenAngle)) < Math::pi_2)
      )
    {
      leftGoalPosition = &(getFieldInfo().opponentGoalPostLeft);
      rightGoalPosition = &(getFieldInfo().opponentGoalPostRight);
    }
    else
    {
      // own goals are switched (!)
      leftGoalPosition = &(getFieldInfo().ownGoalPostRight);
      rightGoalPosition = &(getFieldInfo().ownGoalPostLeft);
    }

    
    /*
    //const ColorClasses::Color opponentGoalColor = (getPlayerInfo().gameData.teamColor == GameData::red)?ColorClasses::skyblue:ColorClasses::yellow;
    if (seenPost.color == opponentGoalColor)
    {
      leftGoalPosition = getFieldInfo().opponentGoalPostLeft;
      rightGoalPosition = getFieldInfo().opponentGoalPostRight;
    }else
    {
      // own goals are switched (!)
      leftGoalPosition = getFieldInfo().ownGoalPostRight;
      rightGoalPosition = getFieldInfo().ownGoalPostLeft;
    }//end else
    */

    for (unsigned int j = 0; j < sampleSet.numberOfParticles; j++)
    { 
      Sample& sample = sampleSet[j];
      Vector2<double> expectedPostPosition;

      // HACK:? each particle decides for itself
      if(fabs(Math::normalize(sample.rotation + seenAngle)) < Math::pi_2)
      {
        leftGoalPosition = &(getFieldInfo().opponentGoalPostLeft);
        rightGoalPosition = &(getFieldInfo().opponentGoalPostRight);
      }
      else
      {
        // own goals are switched (!)
        leftGoalPosition = &(getFieldInfo().ownGoalPostRight);
        rightGoalPosition = &(getFieldInfo().ownGoalPostLeft);
      }



      if(seenPost.type == GoalPercept::GoalPost::rightPost)
      {
        expectedPostPosition = *rightGoalPosition;
        // switch if the robot is behind the goal
        if( (expectedPostPosition.x < 0 && sample.translation.x < expectedPostPosition.x) ||
            (expectedPostPosition.x > 0 && sample.translation.x > expectedPostPosition.x))
          expectedPostPosition = *leftGoalPosition;
      }
      else if(seenPost.type == GoalPercept::GoalPost::leftPost)
      {
        expectedPostPosition = *leftGoalPosition;
        // switch if the robot is behind the goal
        if( (expectedPostPosition.x < 0 && sample.translation.x < expectedPostPosition.x) ||
            (expectedPostPosition.x > 0 && sample.translation.x > expectedPostPosition.x))
          expectedPostPosition = *rightGoalPosition;
      }
      else // unknown post
      {
        Vector2<double> globalPercept = sample * seenPost.position;
        // choose the closest one
        if((globalPercept - *rightGoalPosition).abs() < (globalPercept - *leftGoalPosition).abs())
          expectedPostPosition = *rightGoalPosition;
        else
          expectedPostPosition = *leftGoalPosition;
      }

      Vector2<double> relPost = sample/expectedPostPosition;
      double expectedDistance = relPost.abs();
      double expectedAngle = relPost.angle();

      if(seenPost.positionReliable)
      {
        sample.likelihood *= computeDistanceWeighting(seenDistance, expectedDistance, cameraHeight, sigmaDistance, 1.0);
      }
      sample.likelihood *= computeAngleWeighting(seenAngle, expectedAngle, sigmaAngle, 1.0);

    } //end for j (samples)
  }//end for i (goal posts)

}//end updateByGoalPosts


void MonteCarloSelfLocator::updateByLinesTable(SampleSet& sampleSet) const
{
  const double cameraHeight = getCameraMatrix().translation.z;
  const double sigmaDistance = parameters.sigmaDistanceLine;
  const double sigmaAngle = parameters.sigmaAngleLine;
  double shortestLine = 1e+5; // very long...

  //updateByLinesTableNew();

  //PEN("FF00FF", 10);
  int lp = -1;
  //for(lp=0; lp < getLinePercept().lines.size() && lp < 3; lp++)
  for(int i = 0; i < 3; i++)
  {
    int idx = Math::random((int)getLinePercept().lines.size());
    // dont use the lines which are parts of the circle 
    // when the circle itself was detected
    if(getLinePercept().lines[idx].type == LinePercept::C && getLinePercept().middleCircleWasSeen)
      continue;

    if(getLinePercept().lines[idx].lineOnField.getLength() < 300) // don't use too short lines
      continue;

    lp = idx;
    break;
  }

  if(lp >= 0)
  {
    //const int centerLine_id = 4; // HACK: see FieldInfo
    // special treatment for the center line
    //if(getLinePercept().lines[lp].seen_id == LinePercept::center_line)
    //{
      // get the center line
      //const Math::LineSegment& centerLine = getFieldInfo().fieldLinesTable.getLines()[centerLine_id];
    //}//end if


    // TODO: separate class
    //int lineVotes[30] = {0};
    //int maxIdx = 0;

    const Math::LineSegment& relPercept = getLinePercept().lines[lp].lineOnField;

    for(unsigned int s=0; s < sampleSet.size(); s++)
    {
      Sample& sample = sampleSet[s];

      // statistics
      shortestLine = std::min(shortestLine, relPercept.getLength());


      // translocation of the line percept to the global coords
      Vector2<double> abs_begin = sample*relPercept.begin();
      Vector2<double> abs_end = sample*relPercept.end();
      Vector2<double> abs_direction = abs_end - abs_begin;
      Vector2<double> abs_mid = (abs_begin+abs_end)*0.5;

      // classify the line percept
      int length = (relPercept.getLength() > 700)?LinesTable::long_lines:LinesTable::short_lines|LinesTable::circle_lines|LinesTable::long_lines;
      int direction = (fabs(abs_direction.x) > fabs(abs_direction.y))?LinesTable::along_lines:LinesTable::across_lines;
      int type = (getLinePercept().lines[lp].type == LinePercept::C)?LinesTable::circle_lines:length|direction;

      /*
      const LinesTable::NamedPoint& p_mid = 
        (type&LinesTable::circle_lines)?
          getFieldInfo().fieldLinesTable.get_closest_point_direct(abs_mid, LinesTable::idx_circle):
          getFieldInfo().fieldLinesTable.get_closest_line_point_fast(abs_mid, length, direction);
          */
      // get the closest line in the world
      //LinesTable::NamedPoint p_begin = getFieldInfo().fieldLinesTable.get_closest_point(absPercept.begin(), type);
      //LinesTable::NamedPoint p_end = getFieldInfo().fieldLinesTable.get_closest_point(absPercept.end(), type);
      LinesTable::NamedPoint p_mid = getFieldInfo().fieldLinesTable.get_closest_point(abs_mid, type);

      // there is no such line
      if(p_mid.id == -1)
        continue;

      // get the line
      const Math::LineSegment& ref_line = getFieldInfo().fieldLinesTable.getLines()[p_mid.id];

      /*
      DEBUG_REQUEST("MCSL:draw_corner_votes",
        // vote for the corner id
        ASSERT(p_mid.id <= 29);
        lineVotes[p_mid.id]++;
        if(lineVotes[p_mid.id] > lineVotes[maxIdx]) 
          maxIdx = p_mid.id;
      );
      */
      // project the perceived line to the reference
      Vector2<double> p1 = ref_line.projection(abs_begin);
      Vector2<double> p2 = ref_line.projection(abs_end);
      Vector2<double> pm = p_mid.position;


      DEBUG_REQUEST("MCSL:draw_corner_votes",
        if(getLinePercept().lines[lp].type != LinePercept::C)
        {
          LINE(p1.x, p1.y, pm.x, pm.y);
          LINE(p2.x, p2.y, pm.x, pm.y);
        }
      );

      {
        Vector2<double> relP1(sample/p1);
        sample.likelihood *= computeDistanceWeighting(relPercept.begin().abs(), relP1.abs(), cameraHeight, sigmaDistance, 1.0);
        sample.likelihood *= computeAngleWeighting(relPercept.begin().angle(), relP1.angle(), sigmaAngle, 1.0);
      }
      
      {
        Vector2<double> relP2(sample/p2);
        sample.likelihood *= computeDistanceWeighting(relPercept.end().abs(), relP2.abs(), cameraHeight, sigmaDistance, 1.0);
        sample.likelihood *= computeAngleWeighting(relPercept.end().angle(), relP2.angle(), sigmaAngle, 1.0);
      }
      {
        Vector2<double> relPM(sample/pm);
        Vector2<double> relMidPoint = relPercept.point(0.5*relPercept.getLength());
        sample.likelihood *= computeDistanceWeighting(relMidPoint.abs(), relPM.abs(), cameraHeight, sigmaDistance, 1.0);
        sample.likelihood *= computeAngleWeighting(relMidPoint.angle(), relPM.angle(), sigmaAngle, 1.0);
      }
      
    }//end for

    /*
    DEBUG_REQUEST("MCSL:draw_corner_votes",
      FIELD_DRAWING_CONTEXT;

      PEN("0000FF", 10);
      for(int i = 0; i < 30; i++)
      {
        if(lineVotes[i] > 0)
        {
          Vector2<double> p = getFieldInfo().fieldLinesTable.getLines()[i].begin();
          Vector2<double> q = getFieldInfo().fieldLinesTable.getLines()[i].end();
          Vector2<double> r = (p+q)*0.5;
          LINE(p.x, p.y, q.x, q.y);
          TEXT_DRAWING(r.x+60, r.y+60, lineVotes[i]);
        }
      }//end for

      // the max vote
      PEN("FF0000", 20);
      Vector2<double> p = getFieldInfo().fieldLinesTable.getLines()[maxIdx].begin();
      Vector2<double> q = getFieldInfo().fieldLinesTable.getLines()[maxIdx].end();
      //Vector2<double> r = (p+q)*0.5;
      LINE(p.x, p.y, q.x, q.y);
    );
    */

  }//end for

  shortestLine = shortestLine;
  DEBUG_REQUEST("MCSL:plots",
    PLOT("MonteCarloSelfLocator:shortestLine", shortestLine);
  );
}//end updateByLinesTable



void MonteCarloSelfLocator::updateByCornersTable(SampleSet& sampleSet) const
{
  const double sigmaDistance = parameters.sigmaDistanceCorner;
  const double sigmaAngle    = parameters.sigmaAngleCorner;
  const double cameraHeight  = getCameraMatrix().translation.z;

  for(unsigned int lp=0; lp < getLinePercept().intersections.size(); lp++)
  {
    // TODO: separate class
    const int numberOfCornerVotes = 30;
    vector<int> cornerVotes(numberOfCornerVotes);
    int maxIdx = 0;

    for(unsigned int s=0; s < sampleSet.numberOfParticles; s++)
    {
      Sample& sample = sampleSet[s];

      // current percept
      const Vector2<double>& relPercept = getLinePercept().intersections[lp].getPosOnField();
      // transform the relative percept to the global coordinates
      Vector2<double> absPercept = sample*relPercept;
      
      bool isXorT = ((getLinePercept().intersections[lp].getType() == Math::Intersection::T) ||(getLinePercept().intersections[lp].getType() == Math::Intersection::X));
      bool isL = getLinePercept().intersections[lp].getType() == Math::Intersection::L;

      LinesTable::NamedPoint p;
      if(isXorT) {
        p = getFieldInfo().fieldLinesTable.get_closest_tcrossing_point(absPercept);
      } else if (isL) {
        p = getFieldInfo().fieldLinesTable.get_closest_corner_point(absPercept);
      }else
      {
        // circle or something like this :)
        continue;
      }
      
      DEBUG_REQUEST("MCSL:draw_corner_votes",
        // vote for the corner id
        ASSERT(p.id < numberOfCornerVotes);
        cornerVotes[p.id]++;
        if(cornerVotes[p.id] > cornerVotes[maxIdx]) 
          maxIdx = p.id;
      );


      Vector2<double> relPoint = sample/p.position;

      sample.likelihood *= computeDistanceWeighting(relPercept.abs(), relPoint.abs(), cameraHeight, sigmaDistance, 1.0);
      sample.likelihood *= computeAngleWeighting(relPercept.angle(), relPoint.angle(), sigmaAngle, 1.0);
    }//end for

    DEBUG_REQUEST("MCSL:draw_corner_votes",
      FIELD_DRAWING_CONTEXT;

      PEN("0000FF", 10);
      for(unsigned int i = 0; i < getFieldInfo().fieldLinesTable.getIntersections().size(); i++)
      {
        if(cornerVotes[i] > 0)
        {
          Vector2<double> p = getFieldInfo().fieldLinesTable.getIntersections()[i].pos;
          CIRCLE(p.x, p.y, 50);
          TEXT_DRAWING(p.x+60, p.y+60, cornerVotes[i]);
        }
      }//end for

      // the max vote
      PEN("FF0000", 20);
      Vector2<double> p = getFieldInfo().fieldLinesTable.getIntersections()[maxIdx].pos;
      CIRCLE(p.x, p.y, 50);
    );
  }//end for
}//end updateByCornersTable


void MonteCarloSelfLocator::updateByMiddleCircle(SampleSet& sampleSet) const
{
  if(!getLinePercept().middleCircleWasSeen)
    return;

  double sigmaDistance = parameters.sigmaDistanceCenterCircle;
  double sigmaAngle    = parameters.sigmaAngleCenterCircle;
  double cameraHeight  = getCameraMatrix().translation.z;

  Vector2<double> centerCirclePosition; // (0,0)

  for(unsigned int s=0; s < sampleSet.numberOfParticles; s++)
  {
    Sample& sample = sampleSet[s];

    // translate the center circle to local coord 
    Vector2<double> localCircle = sample/centerCirclePosition;

    double expectedDistance = localCircle.abs();
    double expectedAngle = localCircle.angle();

    double seenDistance = getLinePercept().middleCircleCenter.abs();
    double seenAngle = getLinePercept().middleCircleCenter.angle();

    //double distanceDiff = fabs(expectedDistance - seenDistance);
    //double angleDiff = Math::normalize(seenAngle - expectedAngle);
    //sample.likelihood *= exp(-pow((distanceDiff)/sigmaDistance,2));
    //sample.likelihood *= exp(-pow((angleDiff)/sigmaAngle,2));

    sample.likelihood *= computeDistanceWeighting(seenDistance, expectedDistance, cameraHeight, sigmaDistance, 1.0);
    sample.likelihood *= computeAngleWeighting(seenAngle, expectedAngle, sigmaAngle, 1.0);
  }//end for

}//end updateByMiddleCircle


void MonteCarloSelfLocator::updateByFlags(SampleSet& sampleSet) const
{
  double sigmaDistance = parameters.sigmaDistanceGoalPost;
  double sigmaAngle    = parameters.sigmaAngleGoalPost;
  double cameraHeight  = getCameraMatrix().translation.z;

  for(unsigned int i = 0; i < getLinePercept().flags.size(); i++)
  {
    const LinePercept::Flag& flag = getLinePercept().flags[i];

    for (unsigned int j = 0; j < sampleSet.numberOfParticles; j++)
    {
      Sample& sample = sampleSet[j];

      // translate the flag to local coord 
      Vector2<double> localFlag = sample/flag.absolutePosOnField;

      double expectedDistance = localFlag.abs();
      double expectedAngle = localFlag.angle();

      double seenDistance = flag.seenPosOnField.abs();
      double seenAngle = flag.seenPosOnField.angle();

      sample.likelihood *= computeDistanceWeighting(seenDistance, expectedDistance, cameraHeight, sigmaDistance, 1.0);
      sample.likelihood *= computeAngleWeighting(seenAngle, expectedAngle, sigmaAngle, 1.0);
    
    } //end for j
  }//end for i

}//end updateByFlags


int MonteCarloSelfLocator::sensorResetBySensingGoalModel(SampleSet& sampleSet, int n) const
{
// sensor resetting by whole goal
  if(getSensingGoalModel().someGoalWasSeen)
  {
    
    // currently, getCompassDirection() is in fact just the rotation of the robot pose
    Pose2D pose = getSensingGoalModel().calculatePose(getCompassDirection(), getFieldInfo());

    if(isInsideCarpet(pose.translation))
    {
      sampleSet[n].translation = pose.translation;
      sampleSet[n].rotation = pose.rotation;
      n++;
    }
    
    // HACK: generate a mirrored pose
    if(n < (int)sampleSet.numberOfParticles && !getRobotPose().isValid)
    {
      Pose2D poseMirrored(pose);
      poseMirrored.translation *= -1;
      poseMirrored.rotate(Math::pi); 

      if(isInsideCarpet(poseMirrored.translation))
      {
        sampleSet[n].translation = pose.translation;
        sampleSet[n].rotation = pose.rotation;
        n++;
      }
    }
  }//end if

  return n;
}//end sensorResetBySensingGoalModel


int MonteCarloSelfLocator::sensorResetByGoalPosts(SampleSet& sampleSet, int n) const
{
  double maxDistanceError = 0;
  double maxAngleError = 0;

  // check whether the goals fit to the observations
  if(getGoalPercept().getNumberOfSeenPosts() > 0)
  {
    for(int i = 0; i < getGoalPercept().getNumberOfSeenPosts(); i++)
    {
      const GoalPercept::GoalPost& seenPost = getGoalPercept().getPost(i);

      const double seenDistance = seenPost.position.abs();
      const double seenAngle = seenPost.position.angle();

      //const ColorClasses::Color opponentGoalColor = (getPlayerInfo().gameData.teamColor == GameData::red)?ColorClasses::skyblue:ColorClasses::yellow;

      Vector2<double> leftGoalPosition;
      Vector2<double> rightGoalPosition;

      //if (seenPost.color == opponentGoalColor)
      if(getFieldSidePercept().facedFieldSide == FieldSidePercept::opponent ||
        (getFieldSidePercept().facedFieldSide == FieldSidePercept::unknown && 
        //fabs(Vector2<double>(seenPost.position).rotate(getRobotPose().rotation).angle()) < Math::pi_2)
        fabs(Math::normalize(getRobotPose().rotation + seenPost.position.angle())) < Math::pi_2)
      )
      {
        leftGoalPosition = getFieldInfo().opponentGoalPostLeft;
        rightGoalPosition = getFieldInfo().opponentGoalPostRight;
      }else
      {
        // own goals are switched (!)
        leftGoalPosition = getFieldInfo().ownGoalPostRight;
        rightGoalPosition = getFieldInfo().ownGoalPostLeft;
      }//end else
      

      Vector2<double> expectedPostPosition;

      if(seenPost.type == GoalPercept::GoalPost::rightPost)
      {
        expectedPostPosition = rightGoalPosition;
        // switch if the robot is behind the goal
        if( (expectedPostPosition.x < 0 && getRobotPose().translation.x < expectedPostPosition.x) ||
            (expectedPostPosition.x > 0 && getRobotPose().translation.x > expectedPostPosition.x))
          expectedPostPosition = leftGoalPosition;
      }
      else if(seenPost.type == GoalPercept::GoalPost::leftPost)
      {
        expectedPostPosition = leftGoalPosition;
        // switch if the robot is behind the goal
        if( (expectedPostPosition.x < 0 && getRobotPose().translation.x < expectedPostPosition.x) ||
            (expectedPostPosition.x > 0 && getRobotPose().translation.x > expectedPostPosition.x))
          expectedPostPosition = rightGoalPosition;
      }
      else // unknown post
      {
        Vector2<double> globalPercept = getRobotPose() * seenPost.position;
        // choose the closest one
        if((globalPercept - rightGoalPosition).abs() < (globalPercept - leftGoalPosition).abs())
          expectedPostPosition = rightGoalPosition;
        else
          expectedPostPosition = leftGoalPosition;
      }

      Vector2<double> relPost = getRobotPose()/expectedPostPosition;
      double expectedDistance = relPost.abs();
      double expectedAngle = relPost.angle();

      double distanceError = fabs(seenDistance - expectedDistance);
      double angleError = fabs(Math::normalize(expectedAngle - seenAngle));

      maxDistanceError = max(maxDistanceError, distanceError);
      maxAngleError = max(maxAngleError, angleError);
 
    }//end for i (goal posts)
  }//end if



  // WORK in progress
  if(false && (maxDistanceError > 1000 || maxAngleError > Math::fromDegrees(30)) && getGoalPercept().getNumberOfSeenPosts() > 0)
  {
    PLOT("MCSL:maxDistanceError",maxDistanceError);
    PLOT("MCSL:maxAngleError",maxAngleError);

    // neue samples einstreuen (10 prozent)
    // TODO: draw with the probability of the goal percept
    int numberOfPartiklesToResample = 1;
    for(int i=0; i < numberOfPartiklesToResample; i++) 
    {
      if (n < (int)sampleSet.numberOfParticles) 
      {
          // select a random post
          int idx = Math::random((int)getGoalPercept().getNumberOfSeenPosts());
          const GoalPercept::GoalPost& seenPost = getGoalPercept().getPost(idx);

          const ColorClasses::Color opponentGoalColor = (getPlayerInfo().gameData.teamColor == GameData::red)?ColorClasses::skyblue:ColorClasses::yellow;

          Vector2<double> leftGoalPosition;
          Vector2<double> rightGoalPosition;
          if (seenPost.color == opponentGoalColor)
          {
            leftGoalPosition = getFieldInfo().opponentGoalPostLeft;
            rightGoalPosition = getFieldInfo().opponentGoalPostRight;
          }else
          {
            leftGoalPosition = getFieldInfo().ownGoalPostLeft;
            rightGoalPosition = getFieldInfo().ownGoalPostRight;
          }//end else 

          bool templateAvaliable = false;
          if( seenPost.type == GoalPercept::GoalPost::leftPost || 
             (seenPost.type == GoalPercept::GoalPost::unknownPost && rand() > 0.5)
            )
            templateAvaliable = generateTemplateFromPosition(sampleSet[n], seenPost.position, leftGoalPosition);
          else
            templateAvaliable = generateTemplateFromPosition(sampleSet[n], seenPost.position, rightGoalPosition);
        
          // create a random sample
          if(!templateAvaliable)
          {
            createRandomSample(sampleSet[n]);
          }
        n++;
      }//end if
    }//end for
  }//end if

  return n;
}//end sensorResetByGoals


// copied from BH10
bool MonteCarloSelfLocator::generateTemplateFromPosition(
  Sample& newTemplate,
  const Vector2<double>& posSeen, 
  const Vector2<double>& posReal) const
{
  const double standardDeviationDistance = 150;
  double r = posSeen.abs() + getFieldInfo().goalpostRadius;
  double distUncertainty = Math::sampleTriangularDistribution(standardDeviationDistance);
  if(r+distUncertainty > standardDeviationDistance)
    r += distUncertainty;
  Vector2<double> realPosition = posReal;
  double minY = std::max(posReal.y - r, static_cast<double>(getFieldInfo().yPosLeftSideline));
  double maxY = std::min(posReal.y + r, static_cast<double>(getFieldInfo().yPosRightSideline));
  Vector2<double> p;
  p.y = minY + Math::random()*(maxY - minY);
  double xOffset(sqrt(Math::sqr(r) - Math::sqr(p.y - posReal.y)));
  p.x = posReal.x;
  p.x += (p.x > 0) ? -xOffset : xOffset;

  if(isInsideCarpet(p))
  {
    double origAngle = (realPosition-p).angle();
    double observedAngle = posSeen.angle();
    newTemplate.translation = p;
    newTemplate.rotation = origAngle-observedAngle;
    return true;
  }
  return false;
}//end generateTemplateFromPosition


inline bool MonteCarloSelfLocator::isInsideCarpet(const Vector2<double>& p) const
{
  //Vector2<double> fieldMin(-getFieldInfo().xFieldLength/2.0, -getFieldInfo().yFieldLength/2.0);
  //Vector2<double> fieldMax( getFieldInfo().xFieldLength/2.0,  getFieldInfo().yFieldLength/2.0);

  return p.y > fieldMin.y && p.y < fieldMax.y &&
         p.x > fieldMin.x && p.x < fieldMax.x;
}//end isInsideCarpet

inline void MonteCarloSelfLocator::createRandomSample(Sample& sample) const
{
  //Vector2<double> fieldMin(-getFieldInfo().xFieldLength/2.0, -getFieldInfo().yFieldLength/2.0);
  //Vector2<double> fieldMax( getFieldInfo().xFieldLength/2.0,  getFieldInfo().yFieldLength/2.0);

  sample.translation.x = Math::random(fieldMin.x, fieldMax.x); //(Math::random()-0.5)*getFieldInfo().xFieldLength;
  sample.translation.y = Math::random(fieldMin.y, fieldMax.y); //(Math::random()-0.5)*getFieldInfo().yFieldLength;
  sample.rotation = Math::random(-Math::pi, Math::pi);
}//end createRandomSample


void MonteCarloSelfLocator::resampleGT07(SampleSet& sampleSet, bool noise)
{

  // calculate the weighting (BH paper)
  // not used yet
  /*
  double totalWeighting = 0;
  for(unsigned int i = 0; i < sampleSet.numberOfParticles; i++)
  {
    totalWeighting += sampleSet[i].likelihood;
  }//end for
  const double averageWeighting = totalWeighting / sampleSet.numberOfParticles;
  static double w_slow = averageWeighting;
  static double w_fast = averageWeighting;
  double alpha_slow = 0.0059;
  double alpha_fast = 0.006;

  w_slow += alpha_slow*(averageWeighting - w_slow);
  w_fast += alpha_fast*(averageWeighting - w_fast);

  double pp = max(0.0, 1.0 - w_fast/w_slow);
  DEBUG_REQUEST("MCSL:plots",
    PLOT("MonteCarloSelfLocator:pp", pp);
    PLOT("MonteCarloSelfLocator:averageWeighting", averageWeighting);
  );
  */
  // copy the samples 
  // TODO: use memcopy?
  SampleSet oldSampleSet = sampleSet;
  oldSampleSet.normalize();


  // add a uniform offset for stabilization
  for(unsigned int i = 0; i < sampleSet.numberOfParticles; i++)
  {
    oldSampleSet[i].likelihood += parameters.resamplingThreshhold;
  }//end for
  oldSampleSet.normalize();


  // resample 10% of particles
  int numberOfPartiklesToResample = 1; //(int)(((double)sampleSet.numberOfParticles)*0.05+0.5);
  
  //numberOfPartiklesToResample = (int)pp;

  // don't resample if no global information is available
  //if(getGoalPercept().getNumberOfSeenPosts() + getLinePercept().flags.size() < 2 &&
  //   !getLinePercept().middleCircleWasSeen)
  //{
    // TODO: add crossings as global information
    //numberOfPartiklesToResample = 0;
  //}

  /*
  if(getGoalPercept().getNumberOfSeenPosts() == 0)
    numberOfPartiklesToResample = 0;
    */

  double sum = -Math::random();
  unsigned int count = 0;

  unsigned int m = 0;  // Zaehler durchs Ausgangs-Set
  unsigned int n = 0;  // Zaehler durchs Ziel-Set

  for(m = 0; m < sampleSet.numberOfParticles; m++)
  {
    sum += oldSampleSet[m].likelihood * oldSampleSet.numberOfParticles;

    // select the particle to copy
    while(count < sum && count < oldSampleSet.numberOfParticles)
    {
      if (n >= oldSampleSet.numberOfParticles - numberOfPartiklesToResample) break;
      
      // copy the selected particle
      sampleSet[n] = oldSampleSet[m];
      if(noise)
      {
        sampleSet[n].translation.x += (Math::random()-0.5)*parameters.processNoiseDistance;
        sampleSet[n].translation.y += (Math::random()-0.5)*parameters.processNoiseDistance;
        sampleSet[n].rotation = Math::normalize(sampleSet[n].rotation + (Math::random()-0.5)*parameters.processNoiseAngle);
      }
      
      n++;
      count++;
    }//end while

    if (n >= oldSampleSet.numberOfParticles-numberOfPartiklesToResample) break;
  }//end for



  // sensor resetting by whole goal
  if(n < oldSampleSet.numberOfParticles)
  {
    n = sensorResetBySensingGoalModel(sampleSet, n);
  }

  // sensor resetting by the goal posts
  if(getGoalPercept().getNumberOfSeenPosts() > 0)
  {
    //TODO: does not work properly yet
    //n = sensorResetByGoalPosts(sampleSet, n);
  }
  

  // fill up by copying random samples
  // (shouldn't happen)
  while (n < sampleSet.numberOfParticles) 
  {
    int i = Math::random(sampleSet.numberOfParticles);//(int)(Math::random()*(sampleSet.numberOfParticles-1) + 0.5);
    sampleSet[n] = sampleSet[i];
    n++;
  }//end while

}//end resampleGT07



// Odometry
void MonteCarloSelfLocator::updateByOdometry(SampleSet& sampleSet, bool noise) const
{
  Pose2D odometryDelta = getOdometryData() - lastRobotOdometry;
  for (unsigned int i = 0; i < sampleSet.numberOfParticles; i++)
  {      
    sampleSet[i] += odometryDelta; 
    if(noise)
    {
      sampleSet[i].translation.x += (Math::random()-0.5)*parameters.motionNoiseDistance;
      sampleSet[i].translation.y += (Math::random()-0.5)*parameters.motionNoiseDistance;
      sampleSet[i].rotation = Math::normalize(sampleSet[i].rotation + (Math::random()-0.5)*parameters.motionNoiseAngle);
    }
  }//end for
}//end updateByOdometry


// Fall Down noise
void MonteCarloSelfLocator::updateFallDown(SampleSet& sampleSet) const
{
  for (unsigned int i = 0; i < sampleSet.numberOfParticles; i++)
  {      
    Sample& sample = sampleSet[i];
    //sample.translation.x += (Math::random()-0.5)*parameters.motionNoiseDistance;
    //sample.translation.y += (Math::random()-0.5)*parameters.motionNoiseDistance;
    sample.rotation = Math::normalize(sample.rotation + (Math::random()-0.5)*0.1);
  }//end for
}//end updateFallDown


// not used now
double MonteCarloSelfLocator::getBearingDeviation(const Pose2D& fromPose, const Vector2<double>& toPoint, double measuredBearing) const
{ 
  // returns a value between 0..1:
  // 0   no bearing deviation (0deg)
  // 1   maximum bearing deviation of pi (=180deg)
  double expectedBearing = (toPoint - fromPose.translation).angle() - fromPose.rotation;
  return fabs(Math::normalize(expectedBearing - measuredBearing)) / Math::pi;
}//end getBearingDeviation


// not used now
double MonteCarloSelfLocator::getDistanceDeviation(const Pose2D& fromPose, const Vector2<double>& toPoint, double measuredDistance) const
{
  // returns a value between 0..1:
  // 0   no distance deviation (0mm)
  // 1   maximum distance deviation

  double measuredDistanceAngle = atan2(getCameraMatrix().translation.z, measuredDistance);
  
  double expectedDistance = (fromPose.translation - toPoint).abs();
  double expectedDistanceAngle = atan2(getCameraMatrix().translation.z, expectedDistance);
  
  return fabs(Math::normalize(expectedDistanceAngle - measuredDistanceAngle)) / Math::pi;
}//end getDistanceDeviation


inline double MonteCarloSelfLocator::computeAngleWeighting(
                                double measuredAngle, 
                                double expectedAngle,
                                double standardDeviation, 
                                double bestPossibleWeighting) const
{ 
  double angleDif = Math::normalize(expectedAngle - measuredAngle);
  return Math::gaussianProbability(angleDif, standardDeviation) / bestPossibleWeighting;
}//end computeAngleWeighting


inline double MonteCarloSelfLocator::computeDistanceWeighting(
                                double measuredDistance, 
                                double expectedDistance,
                                double cameraZ,
                                double standardDeviation, 
                                double bestPossibleWeighting) const
{
  const double measuredDistanceAsAngle = atan2(measuredDistance, cameraZ);
  const double expectedDistanceAsAngle = atan2(expectedDistance, cameraZ);

  double angleDif = Math::normalize(expectedDistanceAsAngle - measuredDistanceAsAngle);
  return Math::gaussianProbability(angleDif, standardDeviation) / bestPossibleWeighting;
}//end computeDistanceWeighting



bool MonteCarloSelfLocator::updateBySensors(SampleSet& sampleSet) const
{

  // calculate if sensor data is available
  // ALWAYS SYNCHRONIZE LOGIC WITH hasSensorUpdate
  bool sensorDataAvailable = false;
  
  // I: goals
  if(parameters.updateByGoals)
  {
    STOPWATCH_START("MonteCarloSelfLocator ~ updateByGoals");
    if(getGoalPercept().getNumberOfSeenPosts() > 0)
    {
      updateByGoalPosts(sampleSet);
      sensorDataAvailable = true;
    }
    STOPWATCH_STOP("MonteCarloSelfLocator ~ updateByGoals");
    // update by the goal model
    /*
    if(getSensingGoalModel().someGoalWasSeen)
    {
      updateByGoalModel(sampleSet);
      sensorDataAvailable = true;
    }*/
  }//end update by goals
  

  // II: lines
  if(parameters.updateByLinesTable > 0 && getLinePercept().lines.size() > 0)
  {
    STOPWATCH_START("MonteCarloSelfLocator ~ updateByLines");
    if(parameters.updateByLinesTable > 0)
    {
      updateByLinesTable(sampleSet);
    }
    STOPWATCH_STOP("MonteCarloSelfLocator ~ updateByLines");
    sensorDataAvailable = true;
  }//end update by lines


  // III: corners
  if(parameters.updateByCornerTable > 0 && getLinePercept().intersections.size() > 0)
  {
    STOPWATCH_START("MonteCarloSelfLocator ~ updateByCorners");
    if(parameters.updateByCornerTable > 0)
    {
      updateByCornersTable(sampleSet);
    }
    STOPWATCH_STOP("MonteCarloSelfLocator ~ updateByCorners");
    sensorDataAvailable = true;
  }//end update by corners

  // IV: center circle
  if(parameters.updateByCenterCircle > 0 && getLinePercept().middleCircleWasSeen)
  {
    updateByMiddleCircle(sampleSet);
    sensorDataAvailable = true;
  }//end update by circle


  // V: flags
  if(parameters.updateByFlags && getLinePercept().flags.size() > 0)
  {
    updateByFlags(sampleSet);
    sensorDataAvailable = true;
  }//end update by flags


  if(sensorDataAvailable)
  {
    // update by the old position in order to stabilize it
    if(parameters.updateByOldPose > 0 && initialized 
      // only if the sensing goal model is consistent with the actual guess
      // TODO: is this good? schould we check the validity of it?
      //&& (getSensingGoalModel().calculatePose(getCompassDirection(), getFieldInfo()).translation- getRobotPose().translation).abs() < 700
      && getGoalPercept().getNumberOfSeenPosts() == 0 // no goals seen
      )
    {
      updateByOldPose(sampleSet);
    }
  }//end if


  return sensorDataAvailable;
}//end updateBySensors

bool MonteCarloSelfLocator::hasSensorUpdate() const
{

  // calculate if sensor data is available
  bool sensorDataAvailable = false;

  // goals
  if(parameters.updateByGoals)
  {
    if(getGoalPercept().getNumberOfSeenPosts() > 0)
    {
      sensorDataAvailable = true;
    }
  }//end update by goals


  // lines
  if(parameters.updateByLinesTable > 0 && getLinePercept().lines.size() > 0)
  {
    sensorDataAvailable = true;
  }//end update by lines


  // corners
  if(parameters.updateByCornerTable > 0 && getLinePercept().intersections.size() > 0)
  {
    sensorDataAvailable = true;
  }//end update by corners

  // circle
  if(parameters.updateByCenterCircle > 0 && getLinePercept().middleCircleWasSeen)
  {
    sensorDataAvailable = true;
  }//end update by circle


  // flags
  if(parameters.updateByFlags && getLinePercept().flags.size() > 0)
  {
    sensorDataAvailable = true;
  }//end update by flags

  return sensorDataAvailable;
}//end hasSensorUpdate


void MonteCarloSelfLocator::execute()
{

  /************************************
   * STEP I: treat some special situations:
   ************************************/

  // (I) we know, we are in our half
  static bool init_own_half = false;
  static bool init_opp_half = false;

  if(getSituationStatus().ownHalf)
  {
    // only own half
    //fieldMin = Vector2<double>(-getFieldInfo().xFieldLength/2.0, -getFieldInfo().yFieldLength/2.0);
    //fieldMax = Vector2<double>(                             0.0,  getFieldInfo().yFieldLength/2.0);
    fieldMax.x = 0.0;

    if(!init_own_half)
    {
      resetSampleSet(theSampleSet);
      initialized = false;
    }
    clampSampleSetToField(theSampleSet);
    init_own_half = true;
  }
  else
  {
    //fieldMin = Vector2<double>(-getFieldInfo().xFieldLength/2.0, -getFieldInfo().yFieldLength/2.0);
    //fieldMax = Vector2<double>( getFieldInfo().xFieldLength/2.0,  getFieldInfo().yFieldLength/2.0);
    fieldMax.x = getFieldInfo().xFieldLength/2.0;
    init_own_half = false;
  }
  
  if(getSituationStatus().oppHalf && !getSituationStatus().ownHalf)
  {
    fieldMin.x = 0.0;

    if(!init_opp_half)
    {
      resetSampleSet(theSampleSet);
      initialized = false;
    }
    clampSampleSetToField(theSampleSet);
    init_opp_half = true;
  }
  else
  {
    //fieldMin = Vector2<double>(-getFieldInfo().xFieldLength/2.0, -getFieldInfo().yFieldLength/2.0);
    //fieldMax = Vector2<double>( getFieldInfo().xFieldLength/2.0,  getFieldInfo().yFieldLength/2.0);
    fieldMin.x = -getFieldInfo().xFieldLength/2.0;
    init_opp_half = false;
  }
  // HACK
  //initialized = initialized || getSensingGoalModel().someGoalWasSeen;


  DEBUG_REQUEST("MCSL:reset_samples",
    resetSampleSet(theSampleSet);
    
    DEBUG_REQUEST("MCSL:draw_Samples",
      // draw the random samples
      drawSamplesImportance(theSampleSet);    
    );
    return;
  );

  // (II) treat the situation when the robot fell down
  if(parameters.treatFallDown && (
     getBodyState().fall_down_state != BodyState::upright ||
     getMotionStatus().currentMotion == motion::stand_up_from_back ||
     getMotionStatus().currentMotion == motion::stand_up_from_front)) // robot is not upright
  {
    // apply faling down noise
    updateFallDown(theSampleSet);

    // HACK
    DEBUG_REQUEST("MCSL:draw_position",
      drawPosition();
    );
    return;
  }//end if
  

  // (III) treat the situation when the robot has been lifted from the ground
  // (keednapped)
  if(getMotionStatus().currentMotion == motion::stand && // only in stand (!)
     getBodyState().fall_down_state == BodyState::upright && parameters.treatLiftUp && (
     !getBodyState().standByLeftFoot && !getBodyState().standByRightFoot && // no foot is on the ground
      getFrameInfo().getTimeSince(getBodyState().foot_state_time) > 1000 )) // we lose the ground contact for more then 1s
  {
    resetSampleSet(theSampleSet);

    // HACK
    DEBUG_REQUEST("MCSL:draw_position",
      drawPosition();
    );
    return;
  }//end if


  // reset particles
  theSampleSet.resetLikelihood();

  // check if there is any sensor data
  bool sensorDataAvailable = hasSensorUpdate();


  /************************************
   * STEP II: motion update
   ************************************/
  {
    // apply some noise if sensor data is available 
    updateByOdometry(theSampleSet, sensorDataAvailable);
    lastRobotOdometry = getOdometryData();
  }


  /************************************
   * STEP III: sensor update
   ************************************/
  bool sensorDataAvailableByUpdate = updateBySensors(theSampleSet);
  ASSERT(sensorDataAvailable == sensorDataAvailableByUpdate);

  DEBUG_REQUEST("MCSL:draw_Samples",
    // draw the distribution of the importance 
    // before the particles are resampled
    drawSamplesImportance(theSampleSet);    
  );

  /************************************
   * STEP IV: resampling
   ************************************/
  if(sensorDataAvailable)
  {
    resampleGT07(theSampleSet, true);

    theSampleSet.normalize();
  }//end if
  

  /************************************
   * STEP V: clustering
   ************************************/

  //gridClustering.cluster();

  // try to track the hypothesis
  int clusterSize = canopyClustering.cluster(getRobotPose().translation);
  PLOT("MCSL:clusterSize", clusterSize);
  

  // a heap could collect more than 70% of all particles
  initialized = initialized || (clusterSize > 0.9*(double)theSampleSet.numberOfParticles);
  PLOT("MCSL:initialized", initialized);

  // TODO: find a beter place for it
  getRobotPose().isValid = initialized;

  // Hypothesis tracking:
  // the idea is to keep the cluster until it has at lest 1/3 of all particles
  // if not, then jump only if there is anoter cluster having more then 2/3 particles

  // if the hypothesis is to small...
  if(clusterSize < 0.3*(double)theSampleSet.numberOfParticles)
  {
    // make new cluseter
    canopyClustering.cluster();

    // find the largest cluster
    Moments2<2> tmpMoments;
    Sample tmpPose = theSampleSet.meanOfLargestCluster(tmpMoments);

    // TODO: make it more efficient
    // if it is not suficiently bigger revert the old clustering
    if(tmpMoments.getRawMoment(0,0) < 0.6*(double)theSampleSet.numberOfParticles)
      canopyClustering.cluster(getRobotPose().translation);
    else // jump...
      getRobotPose().isValid = false;
  }//end if

  // TODO: clear plot
  /*
  if(!getRobotPose().isValid)
  {

  }
  */

  /************************************
   * STEP VI: estimate new position and update the model
   ************************************/

  // estimate the deviation of the pose
  Moments2<2> moments;
  Sample newPose = theSampleSet.meanOfLargestCluster(moments);

  Vector2<double> major;
  Vector2<double> minor;
  moments.getAxes(major, minor);

  getRobotPose().principleAxisMajor = major;
  getRobotPose().principleAxisMinor = minor;

  // HACK
  // check if the position jumps
  if(false && (getRobotPose().translation - newPose.translation).abs() > 1000)
  {
    newPose.translation = getRobotPose().translation * 0.9 + newPose.translation * 0.1;
    newPose.rotation = getRobotPose().rotation * 0.9 + newPose.rotation * 0.1;
    getRobotPose().isValid = false;
  }

  // stupid "=" operator (is there a better way to do it?)
  bool wasValid = getRobotPose().isValid;
  getRobotPose() = newPose;
  getRobotPose().isValid = wasValid;

  // update the goal model based on the robot pose
  getSelfLocGoalModel().update(getRobotPose(), getFieldInfo());


  /************************************
   * STEP VII: execude some debug requests (drawings)
   ************************************/

  DEBUG_REQUEST("MCSL:draw_Cluster",
    theSampleSet.drawCluster(newPose.cluster);
  );

  DEBUG_REQUEST("MCSL:draw_visible_lines",
    Math::LineSegment line = getFieldInfo().fieldLinesTable.getNearestLine(getRobotPose());
    
    FIELD_DRAWING_CONTEXT;
    PEN("FF000099", 20);  
    LINE(line.begin().x, line.begin().y,
           line.end().x, line.end().y);
  );

  DEBUG_REQUEST("MCSL:draw_deviation",
    FIELD_DRAWING_CONTEXT;
    PEN("000000", 20);

    LINE(getRobotPose().translation.x - major.x, 
         getRobotPose().translation.y - major.y,
         getRobotPose().translation.x + major.x, 
         getRobotPose().translation.y + major.y);

    LINE(getRobotPose().translation.x - minor.x, 
         getRobotPose().translation.y - minor.y,
         getRobotPose().translation.x + minor.x, 
         getRobotPose().translation.y + minor.y);

    OVAL_ROTATED(getRobotPose().translation.x, 
                 getRobotPose().translation.y, 
                 minor.abs()*2.0,
                 major.abs()*2.0,
                 minor.angle());
  );

  DEBUG_REQUEST("MCSL:draw_position",
    drawPosition();
  );

  DEBUG_REQUEST("MCSL:draw_SelfLocGoalModel",
    drawSelfLocGoalModel();
  );

  DEBUG_REQUEST("MCSL:draw_sensor_belief",
    draw_sensor_belief();
  );

  DEBUG_REQUEST("MCSL:draw_field_lines",
    getFieldInfo().fieldLinesTable.draw();
    
    FIELD_DRAWING_CONTEXT;
    PEN("FF0000", 50);
    CIRCLE(getFieldInfo().opponentGoalPostLeft.x, getFieldInfo().opponentGoalPostLeft.y, 50);
    TEXT_DRAWING(getFieldInfo().opponentGoalPostLeft.x+50, getFieldInfo().opponentGoalPostLeft.y+50, "L");

    CIRCLE(getFieldInfo().opponentGoalPostRight.x, getFieldInfo().opponentGoalPostRight.y, 50);
    TEXT_DRAWING(getFieldInfo().opponentGoalPostRight.x+50, getFieldInfo().opponentGoalPostRight.y+50, "R");


    PEN("000000", 50);
    CIRCLE(getFieldInfo().ownGoalPostLeft.x, getFieldInfo().ownGoalPostLeft.y, 50);
    TEXT_DRAWING(getFieldInfo().ownGoalPostLeft.x+50, getFieldInfo().ownGoalPostLeft.y+50, "L");

    CIRCLE(getFieldInfo().ownGoalPostRight.x, getFieldInfo().ownGoalPostRight.y, 50);
    TEXT_DRAWING(getFieldInfo().ownGoalPostRight.x+50, getFieldInfo().ownGoalPostRight.y+50, "R");
  );

  DEBUG_REQUEST("MCSL:draw_distances:on",
    int type = 0;
    
    DEBUG_REQUEST("MCSL:draw_distances:long", type |= LinesTable::long_lines; );
    DEBUG_REQUEST("MCSL:draw_distances:short", type |= LinesTable::short_lines; );
    DEBUG_REQUEST("MCSL:draw_distances:along", type |= LinesTable::along_lines; );
    DEBUG_REQUEST("MCSL:draw_distances:across", type |= LinesTable::across_lines; );
    DEBUG_REQUEST("MCSL:draw_distances:circle", type |= LinesTable::circle_lines; );
    DEBUG_REQUEST("MCSL:draw_distances:all", type |= LinesTable::all_lines; );

    getFieldInfo().fieldLinesTable.draw_closest_points(type);
  );

  DEBUG_REQUEST("MCSL:draw_corner_distances:L",
    getFieldInfo().fieldLinesTable.draw_closest_corner_points();
  );

  DEBUG_REQUEST("MCSL:draw_corner_distances:TX",
    getFieldInfo().fieldLinesTable.draw_closest_tcrossing_points();
  );

}//end execute



void MonteCarloSelfLocator::resetSampleSet(SampleSet& sampleSet)
{
  initialized = false;

  double likelihood = 1.0/static_cast<double>(sampleSet.numberOfParticles);
  for (unsigned int i = 0; i < sampleSet.numberOfParticles; i++)
  {
    Sample& sample = sampleSet[i];
    createRandomSample(sample);
    sample.likelihood = likelihood;
  }//end for
}//end resetSampleSet


void MonteCarloSelfLocator::clampSampleSetToField(SampleSet& sampleSet)
{
  for (unsigned int i = 0; i < sampleSet.numberOfParticles; i++)
  {
    Sample& sample = sampleSet[i];
    if(!isInsideCarpet(sample.translation))
    {
      createRandomSample(sample);
    }
  }//end for
}//end clampSampleSetToField


void MonteCarloSelfLocator::mirrorSampleSetFieldSides(SampleSet& sampleSet)
{
  for (unsigned int i = 0; i < sampleSet.numberOfParticles; i++)
  {
    Sample& sample = sampleSet[i];
    sample.translation *= -1;
    sample.rotate(Math::pi);
  }//end for
}//end mirrorSampleSetFieldSides


void MonteCarloSelfLocator::drawPosition() const
{
  FIELD_DRAWING_CONTEXT;
  if(getRobotPose().isValid)
  {
    switch( getPlayerInfo().gameData.teamColor )
    {
    case GameData::red:
      PEN("FF0000", 20);
      break;
    case GameData::blue:
      PEN("0000FF", 20);
      break;
    default:
      PEN("AAAAAA", 20);
      break;
    }
  }
  else
  {
    PEN("AAAAAA", 20);
  }

  ROBOT(getRobotPose().translation.x,
        getRobotPose().translation.y,
        getRobotPose().rotation);

  // simple drawing :)
  CIRCLE(getRobotPose().translation.x, 
         getRobotPose().translation.y, 150);

  ARROW(getRobotPose().translation.x, getRobotPose().translation.y, 
        getRobotPose().translation.x + 50*cos(getRobotPose().rotation), 
        getRobotPose().translation.y + 50*sin(getRobotPose().rotation));

  static Vector2<double> oldPose = getRobotPose().translation;
  if((oldPose - getRobotPose().translation).abs() > 100)
  {
    PLOT2D("MCSL:position_trace",getRobotPose().translation.x, getRobotPose().translation.y);
    oldPose = getRobotPose().translation;
  }
}//end drawPosition


void MonteCarloSelfLocator::drawSamplesImportance(SampleSet& sampleSet) const
{
  FIELD_DRAWING_CONTEXT;

  // normalize the colors (black: less importent, red more importent)
  double minValue = 1;
  double maxValue = 0;
  for (unsigned int i = 0; i < sampleSet.numberOfParticles; i++)
  {
    maxValue = max(sampleSet[i].likelihood, maxValue);
    minValue = min(sampleSet[i].likelihood, minValue);
  }
  double colorDiff = maxValue-minValue;

  // plot samples
  for (unsigned int i = 0; i < sampleSet.numberOfParticles; i++)
  {
    DebugDrawings::Color color;
    if(colorDiff > 0)
      color[0] = Math::clamp((sampleSet[i].likelihood - minValue)/colorDiff,0.0,1.0);
    
    PEN(color, 20);

    ARROW(sampleSet[i].translation.x, sampleSet[i].translation.y, 
          sampleSet[i].translation.x + 100*cos(sampleSet[i].rotation), 
          sampleSet[i].translation.y + 100*sin(sampleSet[i].rotation));
    
    /*
    CIRCLE(sampleSet[i].translation.x, sampleSet[i].translation.y, 20);
    LINE( sampleSet[i].translation.x, sampleSet[i].translation.y, 
          sampleSet[i].translation.x + 100*cos(sampleSet[i].rotation), 
          sampleSet[i].translation.y + 100*sin(sampleSet[i].rotation));
    */
  }//end for
}//end drawSamplesImportance


void MonteCarloSelfLocator::drawSelfLocGoalModel() const
{
  // remark: the selfloc goal model always contains the opponent goal
  FIELD_DRAWING_CONTEXT;
  PEN("FF0000", 50);
  CIRCLE(getSelfLocGoalModel().goal.leftPost.x, getSelfLocGoalModel().goal.leftPost.y, 50);
  CIRCLE(getSelfLocGoalModel().goal.rightPost.x, getSelfLocGoalModel().goal.rightPost.y, 50);
  LINE(getSelfLocGoalModel().goal.rightPost.x, getSelfLocGoalModel().goal.rightPost.y, getSelfLocGoalModel().goal.leftPost.x, getSelfLocGoalModel().goal.leftPost.y);
}//end drawSelfLocGoalModel


void MonteCarloSelfLocator::draw_sensor_belief() const
{
  static const int ySize = 20;
  static const int xSize = 30;
  double yWidth = 0.5*getFieldInfo().yFieldLength/(double)ySize;
  double xWidth = 0.5*getFieldInfo().xFieldLength/(double)xSize;

  FIELD_DRAWING_CONTEXT;
  DebugDrawings::Color white(1.0,1.0,1.0,0.0); // transparent
  DebugDrawings::Color black(0.0,0.0,0.0,1.0);


  // create new sample set
  SampleSet sampleSet(xSize*ySize);
  int idx = 0;

  for (int x = 0; x < xSize; x++)
  {
    for (int y = 0; y < ySize; y++)
    {
      Vector2<double> point(xWidth*(2*x-xSize+1), yWidth*(2*y-ySize+1));
      sampleSet.samples[idx].translation = point;
      sampleSet.samples[idx].rotation = getRobotPose().rotation;
      sampleSet.samples[idx].likelihood = 1.0;
      idx++;
    }//end for
  }//end for

  updateBySensors(sampleSet);
  
  double maxValue = 0;
  idx = 0;
  for (int x = 0; x < xSize; x++)
  {
    for (int y = 0; y < ySize; y++)
    {
      maxValue = max(maxValue, sampleSet.samples[idx++].likelihood);
    }//end for
  }//end for

  if(maxValue == 0) return;

  idx = 0;
  for (int x = 0; x < xSize; x++)
  {
    for (int y = 0; y < ySize; y++)
    {
      Vector2<double> point(xWidth*(2*x-xSize+1), yWidth*(2*y-ySize+1));
      
      double t = sampleSet.samples[idx++].likelihood / maxValue;
      DebugDrawings::Color color = black*t + white*(1-t);
      PEN(color, 20);
      FILLBOX(point.x - xWidth, point.y - yWidth, point.x+xWidth, point.y+yWidth);
    }//end for
  }//end for
}//end draw_closest_points
