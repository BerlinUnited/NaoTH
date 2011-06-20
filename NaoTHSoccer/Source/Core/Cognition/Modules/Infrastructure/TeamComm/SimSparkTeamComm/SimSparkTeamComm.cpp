/**
 * @file SimSparkTeamComm.cpp
 *
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
 *
 */

#include "SimSparkTeamComm.h"

SimSparkTeamComm::SimSparkTeamComm()
{
}

SimSparkTeamComm::~SimSparkTeamComm()
{
}

void SimSparkTeamComm::execute()
{
  readMessage();
  sendMessage();
}

void SimSparkTeamComm::readMessage()
{
  if ( !getTeamMessageData().data.empty() )
    cout<<"read: \n";
  for(vector<string>::const_iterator i=getTeamMessageData().data.begin();
      i!=getTeamMessageData().data.end(); ++i)
  {
    cout<<*i<<'\n';
  }
}

void SimSparkTeamComm::sendMessage()
{
  getRobotMessageData().data = "hello";
}
