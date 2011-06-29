/**
* @file PlayersLocator.h
*
* @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Implementation of class PlayersLocator
*/
#include <limits>
#include "PlayersLocator.h"
// Debug
#include "Tools/Debug/DebugRequest.h"
#include "Tools/Debug/DebugDrawings.h"
#include "Tools/Math/Geometry.h"
#include "Tools/Debug/DebugModify.h"

PlayersLocator::PlayersLocator()
{
  DEBUG_REQUEST_REGISTER("PlayersLocator:draw_seen_players","draw seen robots position", false);
  DEBUG_REQUEST_REGISTER("PlayersLocator:draw_free_regions","draw free regions", false);
  DEBUG_REQUEST_REGISTER("PlayersLocator:draw_players_model","draw the model of robots", false);
  DEBUG_REQUEST_REGISTER("PlayersLocator:draw_closest_player","mark the robots which is closest to ball in team", false);
}


void PlayersLocator::execute()
{
  updateByPlayersPercept();
  updateByTeamMessage();

  // update myself
  PlayersModel::Player& me = getPlayersModel().teammates[getPlayerInfo().gameData.playerNumber];
  me.globalPose = getRobotPose();
  me.pose = Pose2D();
  me.frameInfoWhenWasSeen = getFrameInfo();
  me.number = getPlayerInfo().gameData.playerNumber;
  

  getPlayersModel().oppClosestToBall = calculateClosestPlayerToBall(getPlayersModel().opponents);
  getPlayersModel().ownClosestToBall = calculateClosestPlayerToBall(getPlayersModel().teammates);

  DEBUG_REQUEST("PlayersLocator:draw_free_regions",
    
    /*
    freeRegions.clear();
    freeRegions.push_back(Interval(-Math::pi*0.5, Math::pi*0.5));

    double robot_size = 200.0;
    std::list<PlayersPercept::Player>::const_iterator iter = getPlayersPercept().playersList.begin();
    for(;iter != getPlayersPercept().playersList.end(); iter++)
    {
      // calculate the angle region [a_min, a_max] occupied by this robot
      double a = iter->pose.translation.angle();
      double robot_size_angle = asin(robot_size/iter->pose.translation.abs());
      double a_min = a - robot_size_angle;
      double a_max = a + robot_size_angle;
      
      std::list<Interval>::iterator interval = freeRegions.begin();
      for(;interval != freeRegions.end(); interval++)
      {
        if(interval->in(a_min) || interval->in(a_max))
        {
          if(interval->in(a_min))
          {
            freeRegions.push_front(Interval(interval->a, a_min));
          }

          if(interval->in(a_max))
          {
            freeRegions.push_front(Interval(a_max, interval->b));
          }

          freeRegions.erase(interval);
        }
      }//end for
    }//end for

    PEN("0000FF", 20);
    std::list<Interval>::iterator interval = freeRegions.begin();
    for(;interval != freeRegions.end(); interval++)
    {
      LINE(0,  0, 
           1000*cos(interval->a), 
           1000*sin(interval->a));
      LINE(0,  0, 
           1000*cos(interval->b), 
           1000*sin(interval->b));
    }//end for
    */
  );

  DEBUG_REQUEST("PlayersLocator:draw_seen_players",
    FIELD_DRAWING_CONTEXT;
    
    std::list<PlayersPercept::Player>::const_iterator iter = getPlayersPercept().playersList.begin();
    for(;iter != getPlayersPercept().playersList.end(); ++iter)
    {
      if(iter->teamColor == GameData::blue)
        PEN("0000FF", 20);
      else
        PEN("FF0000", 20);
      ROBOT(iter->pose.translation.x, iter->pose.translation.y, iter->pose.rotation);
    }//end for
  );

  DEBUG_REQUEST("PlayersLocator:draw_players_model",
    FIELD_DRAWING_CONTEXT;

    unsigned int outdate = 20;
  
    for ( std::vector<PlayersModel::Player>::const_iterator iter = getPlayersModel().opponents.begin();
      iter != getPlayersModel().opponents.end(); ++iter)
    {
      if (getFrameInfo().getFrameNumber() - iter->frameInfoWhenWasSeen.getFrameNumber() < outdate)
      {
        if (getPlayerInfo().gameData.teamColor == GameData::red)
          PEN("0000FF", 20);
        else
          PEN("FF0000", 20);
        ROBOT(iter->globalPose.translation.x, iter->globalPose.translation.y, iter->globalPose.rotation);
      }//end if
    }//end for

    for (std::vector<PlayersModel::Player>::const_iterator iter = getPlayersModel().teammates.begin();
      iter != getPlayersModel().teammates.end(); ++iter)
    {
      if (getFrameInfo().getFrameNumber() - iter->frameInfoWhenWasSeen.getFrameNumber() < outdate)
      {
        if (getPlayerInfo().gameData.teamColor == GameData::red)
          PEN("FF0000", 20);
        else
          PEN("0000FF", 20);
        ROBOT(iter->globalPose.translation.x, iter->globalPose.translation.y, iter->globalPose.rotation);
      }//end if
    }//end for
  );

  DEBUG_REQUEST("PlayersLocator:draw_closest_player",
    FIELD_DRAWING_CONTEXT;
    // draw the teammate closest to ball
    if ( getPlayersModel().ownClosestToBall.number != 0 )
    {
      if (getPlayerInfo().gameData.teamColor == GameData::red)
        PEN("FF0000", 20);
      else
        PEN("0000FF", 20);
      const Vector2<double>& p = getPlayersModel().ownClosestToBall.globalPose.translation;
      CIRCLE(p.x, p.y, 180);
      PEN("FFFFFF",20);
      TEXT_DRAWING(p.x, p.y, getPlayersModel().ownClosestToBall.number);
    }//end if

    // draw the opponent closest to ball
    if (getPlayersModel().oppClosestToBall.number != 0)
    {
      if (getPlayerInfo().gameData.teamColor == GameData::red)
        PEN("0000FF", 20);
      else
        PEN("FF0000", 20);
      const Vector2<double>& p = getPlayersModel().oppClosestToBall.globalPose.translation;
      CIRCLE(p.x, p.y, 180);
      PEN("FFFFFF",20);
      TEXT_DRAWING(p.x, p.y, getPlayersModel().oppClosestToBall.number);
    }//end if
  );
}//end execute


void PlayersLocator::updateByPlayersPercept()
{
  if ( getPlayersPercept().theFrameInfo.getFrameNumber() != getFrameInfo().getFrameNumber() )
  {
    return;
  }
  
  GameData::TeamColor oppColor = !getPlayerInfo().gameData.teamColor;

  std::list<PlayersPercept::Player>::const_iterator iter = getPlayersPercept().playersList.begin();
  for(;iter != getPlayersPercept().playersList.end(); ++iter)
  {
    PlayersModel::Player p;
    p.pose = iter->pose;
    p.globalPose = getRobotPose() + iter->pose;
    p.frameInfoWhenWasSeen = getFrameInfo();
    p.number = iter->number;
    if(iter->teamColor == oppColor)
    {
      getPlayersModel().opponents[p.number] = p;
    }
    else
    {
      getPlayersModel().teammates[p.number] = p;
    }//end if

  }//end for
}//end updateByPlayersPercept


void PlayersLocator::updateByTeamMessage()
{

  for(std::map<unsigned int, TeamMessage::Data>::const_iterator iter =getTeamMessage().data.begin();
        iter != getTeamMessage().data.end(); ++iter)
  {
    const naothmessages::TeamCommMessage& message = iter->second.message;
    const int seen_number = iter->first;
    const FrameInfo& messageFrameInfo = iter->second.frameInfo;
  
    // don't consider own messages
    if ( seen_number == getPlayerInfo().gameData.playerNumber ) continue;
    
    
    //TODO: this parameter is only for simspark!!!
    const unsigned int comm_delay = 40;

    if (message.has_positiononfield()
      && getPlayersModel().teammates[seen_number].frameInfoWhenWasSeen.getFrameNumber() < messageFrameInfo.getFrameNumber()-1 )
      {
        PlayersModel::Player& p = getPlayersModel().teammates[seen_number];
        p.globalPose.rotation = message.positiononfield().rotation();
        p.globalPose.translation.x = message.positiononfield().translation().x();
        p.globalPose.translation.y = message.positiononfield().translation().y();
        p.frameInfoWhenWasSeen = 
          FrameInfo(messageFrameInfo.getTime() - comm_delay, 
                    messageFrameInfo.getFrameNumber()-2);
        p.pose = getRobotPose().invert().conc(p.globalPose);
    }//end if

    // update opponents
    if (message.has_opponent())
    {
      unsigned int numOpp = message.opponent().number();

      // Note: the opponent should be seen at least once
      if (getPlayersModel().opponents[numOpp].frameInfoWhenWasSeen.getFrameNumber() < messageFrameInfo.getFrameNumber()-1 )
      {
        PlayersModel::Player& p = getPlayersModel().opponents[numOpp];
        p.globalPose.rotation = message.opponent().poseonfield().rotation();
        p.globalPose.translation.x = message.opponent().poseonfield().translation().x();
        p.globalPose.translation.y = message.opponent().poseonfield().translation().y();
        p.frameInfoWhenWasSeen = 
          FrameInfo(messageFrameInfo.getTime() - comm_delay, 
                    messageFrameInfo.getFrameNumber()-2);
        p.pose = getRobotPose().invert().conc(p.globalPose);
      }
    }//end if
  }//end for
}//end updateByTeamMessage


PlayersModel::Player PlayersLocator::calculateClosestPlayerToBall(const vector<PlayersModel::Player>& team) const
{
  double minimalDistance = std::numeric_limits<double>::infinity();
  PlayersModel::Player theOne;
  for(vector<PlayersModel::Player>::const_iterator iter = team.begin();
    iter != team.end(); ++iter)
  {
    if (iter->frameInfoWhenWasSeen.getFrameNumber() == getFrameInfo().getFrameNumber())
    {
      double distanceToBall = (iter->pose.translation - getBallModel().position).abs2(); // abs2() is faster than abs()
      if (distanceToBall < minimalDistance)
      {
        theOne = *iter;
        minimalDistance = distanceToBall;
      }
    }
  }//end for

  return theOne;
}//end calculateClosestPlayerToBall
