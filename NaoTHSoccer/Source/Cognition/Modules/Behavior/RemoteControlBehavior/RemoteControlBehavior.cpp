/**
 * @file RemoteControlBehavior.cpp
 *
 * @author <a href="mailto:berndale@informatik.hu-berlin.de">Alexander Berndt </a>
 * Implementation of class RemoteControlBehavior
 */

#include "RemoteControlBehavior.h"
#include <Tools/DataConversion.h>

RemoteControlBehavior::RemoteControlBehavior() 
	: state(standby)
{
  REGISTER_DEBUG_COMMAND("remoteControlRequest_WALK",  "remoteControl", this);
  REGISTER_DEBUG_COMMAND("remoteControlRequest_STAND", "remoteControl", this);
  REGISTER_DEBUG_COMMAND("remoteControlRequest_KICK", "remoteControl", this);
  REGISTER_DEBUG_COMMAND("remoteControlRequest_STANDBY", "remoteControl", this);
}

void RemoteControlBehavior::execute() 
{
	
	switch(getRemoteControlCommand().action) 
	{
	case RemoteControlCommand::NONE:
		getMotionRequest().armMotionRequest.id = ArmMotionRequest::arms_none;
		getMotionRequest().id = motion::init;
		break;
	case RemoteControlCommand::STAND: 
		//getMotionRequest().armMotionRequest.id = ArmMotionRequest::arms_down;
		getMotionRequest().id = motion::stand;
		break;
	case RemoteControlCommand::WALK:
		getMotionRequest().armMotionRequest.id = ArmMotionRequest::arms_back;
		getMotionRequest().id = motion::walk;
		getMotionRequest().walkRequest.target = getRemoteControlCommand().target;
		break;
	case RemoteControlCommand::LOCK_ON_BALL:
		//getMotionRequest().armMotionRequest.id = ArmMotionRequest::arms_back;
		//getMotionRequest().id = motion::kick;
		//state = stand;
		break;
	case RemoteControlCommand::KICK:
		break;
	default:
		break;
	}
}//end execute

void RemoteControlBehavior::executeDebugCommand(
    const std::string& command, const ArgumentMap& arguments,
    std::ostream& /*outstream*/)
{
	/*if(command == "remoteControlRequest_WALK")
	{
		double x = 0;
		double y = 0;
		double alpha = 0;
		ArgumentMap::const_iterator iter = arguments.find("x");
		if(iter != arguments.end()) 
		{
			DataConversion::strTo(iter->second, x);
		}
		iter = arguments.find("y");
		if(iter != arguments.end())
		{
			DataConversion::strTo(iter->second, y);
		}
		iter = arguments.find("alpha");
		if(iter != arguments.end())
		{
			DataConversion::strTo(iter->second, alpha);
		}
		walkParams.translation.x = x;
		walkParams.translation.y = y;
		walkParams.rotation = alpha;
		state = walk;
	}
	else if(command == "remoteControlRequest_STAND")
	{
		walkParams.translation.x = 0;
		walkParams.translation.y = 0;
		walkParams.rotation = 0;
		state = stand;
	}
	else if(command == "remoteControlRequest_KICK")
	{
		state = kick;
	}
	else if(command == "remoteControlRequest_STANDBY")
	{
		state = standby;
	}*/
}
