/* 
 * File:   XABSLBehaviorControl.cpp
 * Author: thomas
 * 
 * Created on 9. Januar 2009, 21:31
 */

#include "XABSLBehaviorControl2011.h"

#include "Tools/Xabsl/EngineFactory.h"
#include <DebugCommunication/DebugCommandManager.h>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <PlatformInterface/Platform.h>

using namespace std;

#define REGISTER_MODULE(module, enable) \
  the##module = registerModule<module>(std::string(#module), enable)


XABSLBehaviorControl2011::XABSLBehaviorControl2011()
  : 
    // init the xabxl engine
    theEngine(NULL),
    theErrorHandler(),
    agentName("soccer")
{
  REGISTER_DEBUG_COMMAND("behavior:status", "send the active XABSL options", this);
  REGISTER_DEBUG_COMMAND("behavior:reload", "reload the behavior file", this);
  REGISTER_DEBUG_COMMAND("behavior:get_agent", "get the current selected agent", this);
  REGISTER_DEBUG_COMMAND("behavior:set_agent", "behavior:set_agent agent=<agent_name>", this);
  REGISTER_DEBUG_COMMAND("behavior:list_agents", "return the list of all avaliable agents", this);
  REGISTER_DEBUG_COMMAND("behavior:get_registered_symbols", "return the list of registered symbols", this);
  REGISTER_DEBUG_COMMAND("behavior:get_symbol",
    "return the value of a symbol. usage behavior:get_symbol <symbol_name>", this);

  DEBUG_REQUEST_REGISTER("XABSL:update_status", "update the debug behavior-status", false);

  DEBUG_REQUEST_REGISTER("XABSL:draw_motion_request", "draw the motion intention of the robot on the field", false);
  DEBUG_REQUEST_REGISTER("XABSL:draw_foot_decision", "visualize the foot decision", false);


  // init symbols
  REGISTER_MODULE(BallSymbols2011, true);
  REGISTER_MODULE(GameSymbols2011, true);
  REGISTER_MODULE(MotionSymbols2011, true);
  REGISTER_MODULE(TeamSymbols2011, true);
  REGISTER_MODULE(SensorSymbols2011, true);
  REGISTER_MODULE(MathSymbols2011, true);
  REGISTER_MODULE(GoalSymbols2011, true);
  REGISTER_MODULE(LedSymbols2011, true);
  REGISTER_MODULE(SelflocSymbols2011, true);
  REGISTER_MODULE(OdometrySymbols2011, true);
  REGISTER_MODULE(FieldSymbols2011, true);
  REGISTER_MODULE(StrategySymbols2011, true);
  REGISTER_MODULE(SoundSymbols2011, true);
  REGISTER_MODULE(LineSymbols2011, true);
    

  // load the behavior from config
  if(naoth::Platform::getInstance().theConfiguration.hasKey("agent", "agent"))
  {
    agentName = naoth::Platform::getInstance().theConfiguration.getString("agent", "agent");
  }
  else
  {
    std::cerr << "could not load \"Config/behavior/agent.cfg\", defaulting to \"Soccer\" agent" << std::endl;
  }

  const string behaviorPath = naoth::Platform::getInstance().theConfigDirectory + "/behavior-ic.dat";
  reloadBehaviorFromFile(behaviorPath, agentName);

}//end constructor

XABSLBehaviorControl2011::~XABSLBehaviorControl2011()
{
  delete theEngine;
}//end XABSLBehaviorControl


void XABSLBehaviorControl2011::reloadBehaviorFromFile(std::string file, std::string agent)
{
  delete theEngine;

  //new xabsl::Engine(theErrorHandler, &NaoTime::getNaoTimeInMilliSeconds);
  theEngine = xabsl::EngineFactory<XABSLBehaviorControl2011>::create(theErrorHandler, this->xabslTime);
  
  registerXABSLSymbols();

  XabslFileInputSource input(file);
  theEngine->createOptionGraph(input);
 
  agentName = agent;
  std::cout << "loading agent \"" << agentName << "\" for behavior" << std::endl;
  theEngine->setSelectedAgent(agentName.c_str());
}//end reloadBehaviorFromFile

void XABSLBehaviorControl2011::execute()
{
  // ATTENTION: it has to be set before xabsl engine is executed!!!
  xabslTime = getFrameInfo().getTime();

  // execute the behavior
  if(theEngine != NULL && !theErrorHandler.errorsOccurred)
  {
    updateXABSLSymbols();
    theEngine->execute();
    updateOutputSymbols();
    //cout << "test " << getMotionStatus().motionRequest << endl;

    // update the behavior status
    DEBUG_REQUEST("XABSL:update_status",
      getBehaviorStatus().status.Clear();
      getBehaviorStatus().status.set_agent(agentName);
      getBehaviorStatus().status.set_framenumber(getFrameInfo().getFrameNumber());
      fillActiveOptions(getBehaviorStatus().status);
      fillRegisteredSymbols(getBehaviorStatus().status);
    );

    draw();
  }//end if
}//end execute


void XABSLBehaviorControl2011::draw()
{
  DEBUG_REQUEST("XABSL:draw_foot_decision",
    //18.02.2012
    Vector2<double> oppGoal = getSelfLocGoalModel().getOppGoal(getCompassDirection(), getFieldInfo()).calculateCenter();
    Vector2<double> ballPose = getBallModel().position;

    FIELD_DRAWING_CONTEXT;

    PEN("FFFFFF", 20);
    CIRCLE(oppGoal.x, oppGoal.y, 50);
    LINE(0,0,oppGoal.x,oppGoal.y);

    // normal vector to the RIGHT side
    oppGoal.rotateRight().normalize(200);

    ARROW(0,0, oppGoal.x, oppGoal.y);

    bool kick_with_right_foot = oppGoal*ballPose > 0;

    if(kick_with_right_foot)
    {
      TEXT_DRAWING( - 100, - 100, "RIGHT");
    }
    else
    {
      TEXT_DRAWING( - 100, - 100, "LEFT");
    }
  );


  DEBUG_REQUEST("XABSL:draw_motion_request",
    FIELD_DRAWING_CONTEXT;
    switch(getMotionStatus().currentMotion)
    {
      case motion::kick:
        PEN("FF0000", 20);
        ARROW(getMotionRequest().kickRequest.kickPoint.x, 
              getMotionRequest().kickRequest.kickPoint.y, 
              getMotionRequest().kickRequest.kickPoint.x
                + 400*cos(getMotionRequest().kickRequest.kickDirection), 
              getMotionRequest().kickRequest.kickPoint.y
                + 400*sin(getMotionRequest().kickRequest.kickDirection));

        TEXT_DRAWING(getMotionRequest().kickRequest.kickPoint.x - 100,
             getMotionRequest().kickRequest.kickPoint.y - 100,
             "KICK");
        break;

      case motion::walk:
      {
        PEN("333333", 20);
        Vector2<double> target(100,0);
        target = getMotionRequest().walkRequest.target*target;

        ARROW(getMotionRequest().walkRequest.target.translation.x, 
              getMotionRequest().walkRequest.target.translation.y, 
              target.x, 
              target.y);
        TEXT_DRAWING(
             getMotionRequest().walkRequest.target.translation.x - 100,
             getMotionRequest().walkRequest.target.translation.y - 100,
             "WALK");
      }
      break;
      default: break;
    }//end switch
  );
}//end draw


#define XABSL_REGISTER_SYMBOLS(module) \
  the##module->getModuleT()->registerSymbols(*theEngine)

void XABSLBehaviorControl2011::registerXABSLSymbols()
{
  if(theEngine != NULL)
  {
    XABSL_REGISTER_SYMBOLS(BallSymbols2011);
    XABSL_REGISTER_SYMBOLS(GameSymbols2011);
    XABSL_REGISTER_SYMBOLS(MotionSymbols2011);
    XABSL_REGISTER_SYMBOLS(TeamSymbols2011);
    XABSL_REGISTER_SYMBOLS(SensorSymbols2011);
    XABSL_REGISTER_SYMBOLS(MathSymbols2011);
    XABSL_REGISTER_SYMBOLS(GoalSymbols2011);
    XABSL_REGISTER_SYMBOLS(LedSymbols2011);
    XABSL_REGISTER_SYMBOLS(SelflocSymbols2011);
    XABSL_REGISTER_SYMBOLS(OdometrySymbols2011);
    XABSL_REGISTER_SYMBOLS(FieldSymbols2011);
    XABSL_REGISTER_SYMBOLS(StrategySymbols2011);
    XABSL_REGISTER_SYMBOLS(SoundSymbols2011);
    XABSL_REGISTER_SYMBOLS(LineSymbols2011);
  }//end if
}//end registerXABSLSymbols


void XABSLBehaviorControl2011::updateXABSLSymbols()
{
  if(theEngine != NULL)
  {
    theSelflocSymbols2011->execute();
    theSensorSymbols2011->execute();
    theBallSymbols2011->execute();
    theOdometrySymbols2011->execute();
    theMotionSymbols2011->execute();
    theLineSymbols2011->execute();
  }//end if
}//end updateXABSLSymbols

void XABSLBehaviorControl2011::updateOutputSymbols()
{
  if(theEngine != NULL)
  {
    theMotionSymbols2011->getModuleT()->updateOutputSymbols();
  }
}

// ERROR HANDLER //

MyErrorHandler2011::MyErrorHandler2011()
{
  
}

void MyErrorHandler2011::printError(const char* txt)
{
  std::cerr << "XABSL error: " << txt << std::endl;
}

void MyErrorHandler2011::printMessage(const char* txt)
{
  std::cout << "XABSL message: " << txt << std::endl;
}

MyErrorHandler2011::~MyErrorHandler2011()
{

}


void XABSLBehaviorControl2011::executeDebugCommand(
    const std::string& command, 
    const std::map<std::string, std::string>& arguments,
    std::ostream & outstream)
{
  if(command == "behavior:reload")
  {
    // restart the behavior
    const string behaviorPath = naoth::Platform::getInstance().theConfigDirectory + "/behavior-ic.dat";
    reloadBehaviorFromFile(behaviorPath, agentName);

    outstream << "behavior reloaded";
  }
  else if(command == "behavior:status")
  {
    google::protobuf::io::OstreamOutputStream buf(&outstream);
    getBehaviorStatus().status.SerializeToZeroCopyStream(&buf);
  }
  else if(command == "behavior:get_agent")
  {
    outstream << agentName;
  }
  else if(command == "behavior:list_agents")
  {
    if(theEngine == NULL) return;

    for(int i = 0; i < theEngine->getAgents().getSize(); i++)
    {
      outstream << ((i>0)?"\n":"") << theEngine->getAgents()[i]->n;
    }
  }
  else if(command == "behavior:set_agent")
  {
    if(arguments.find("agent") != arguments.end())
    {
      std::string newAgent = arguments.find("agent")->second;
      if(theEngine == NULL) return;

      if(theEngine->setSelectedAgent(newAgent.c_str()))
      {
        agentName = newAgent;
        outstream << "resetted agent to \"" << agentName << "\"";
      }
      else
      {
        outstream << "agent not found:  \"" << newAgent << "\"";
      }
    }
    else
    {
      outstream << "no \"agent\" parameter given";
    }
  }
  else if(command == "behavior:get_symbol")
  {
    // TODO: return a specific symbol
  }
}//end executeDebugCommand


void XABSLBehaviorControl2011::fillActiveOptions(naothmessages::BehaviorStatus &status)
{
  const xabsl::Array<xabsl::Action*>& actions = theEngine->getRootActions();
  for (int i=0; i < actions.getSize(); i++)
  {
    naothmessages::XABSLAction* action = status.add_activerootactions();
    fillAction(actions[i], action);
  }//end for

} // end fillActiveOptions


void XABSLBehaviorControl2011::fillAction(const xabsl::Action* source, naothmessages::XABSLAction* dest)
{
  if (const xabsl::Behavior* behavior = source->getBehavior())
  {
    const xabsl::Option* option = source->getOption();
    const xabsl::ParameterAssignment* parameters = source->getParameters();

    dest->set_name(behavior->n);
    dest->set_timeofexecution(behavior->timeOfExecution);

    if (option)
    {
      dest->set_activestate(option->activeState->n);
      dest->set_statetime(option->activeState->timeOfStateExecution);
      dest->set_type(naothmessages::XABSLAction_ActionType_Option);
    }
    else
    {
      dest->set_type(naothmessages::XABSLAction_ActionType_BasicBehavior);
    }//end if

    for (int j = 0; j < parameters->decimalValues.getSize(); j++)
    {
      naothmessages::XABSLParameter* p = dest->add_parameters();
      p->set_name(parameters->decimalValues.getName(j));
      p->set_type(naothmessages::XABSLParameter_ParamType_Decimal);
      p->set_decimalvalue(parameters->decimalValues[j]);
    }
    for (int j = 0; j < parameters->booleanValues.getSize(); j++)
    {
      naothmessages::XABSLParameter* p = dest->add_parameters();
      p->set_name(parameters->booleanValues.getName(j));
      p->set_type(naothmessages::XABSLParameter_ParamType_Boolean);
      p->set_boolvalue(parameters->booleanValues[j]);
    }
    for (int j = 0; j < parameters->enumeratedValues.getSize(); j++)
    {
      naothmessages::XABSLParameter* p = dest->add_parameters();
      p->set_name(parameters->enumeratedValues.getName(j));
      p->set_type(naothmessages::XABSLParameter_ParamType_Enum);
      
      p->set_enumvalue("-"); // TODO: output the number "parameters->enumeratedValues[j]" as default

      // find the name for enum value
      const xabsl::Enumeration* enumeration = parameters->enumeratedExpressions[j]->enumeration;
      for (int k = 0; k < enumeration->enumElements.getSize(); k++)
      {
        if (enumeration->enumElements[k]->v == parameters->enumeratedValues[j])
        {
          p->set_enumvalue(enumeration->enumElements[k]->n);
          break;
        }
      }//end for k
    }//end for j


    // stream out children options
    const xabsl::Array<xabsl::Action*>& actions = option->activeState->actions;
    for(int j=0; j < actions.getSize(); j++)
    {
      naothmessages::XABSLAction* newAction = dest->add_activesubactions();
      fillAction(actions[j], newAction);
    }//end for
  }
  else if(const xabsl::DecimalOutputSymbol* symbol = source->getDecimalOutputSymbol())
  {
    dest->set_name(symbol->n);
    dest->set_type(naothmessages::XABSLAction_ActionType_DecimalOutputSymbol);
    dest->set_decimalvalue(symbol->getValue());
  }
  else if(const xabsl::BooleanOutputSymbol* symbol = source->getBooleanOutputSymbol())
  {
    dest->set_name(symbol->n);
    dest->set_type(naothmessages::XABSLAction_ActionType_BooleanOutputSymbol);
    dest->set_boolvalue(symbol->getValue());
  }
  else if(const xabsl::EnumeratedOutputSymbol* symbol = source->getEnumeratedOutputSymbol())
  {
    dest->set_name(symbol->n);
    dest->set_type(naothmessages::XABSLAction_ActionType_EnumOutputSymbol);
    dest->set_enumvalue("-"); // TODO: output the number "symbol->getValue()" as default

    // find the name for enum value
    const xabsl::Enumeration* enumeration = symbol->enumeration;
    for (int k = 0; k < enumeration->enumElements.getSize(); k++)
    {
      if (enumeration->enumElements[k]->v == symbol->getValue())
      {
        dest->set_enumvalue(enumeration->enumElements[k]->n);
        break;
      }
    }//end for
  }//end else if
  
} // end fillActiveOptions


void XABSLBehaviorControl2011::fillRegisteredSymbols(naothmessages::BehaviorStatus &status)
{
  /*******************************************************************
   * input symbols
   *******************************************************************/
  
  // decimalInputSymbols
  for (int i = 0; i < theEngine->decimalInputSymbols.getSize(); i++)
  {
    naothmessages::XABSLParameter* p = status.add_inputsymbols();
    p->set_name(theEngine->decimalInputSymbols[i]->n);
    p->set_type(naothmessages::XABSLParameter_ParamType_Decimal);
    p->set_decimalvalue(theEngine->decimalInputSymbols[i]->getValue());
  }//end for

  // booleanInputSymbols
  for (int i = 0; i < theEngine->booleanInputSymbols.getSize(); i++)
  {
    naothmessages::XABSLParameter* p = status.add_inputsymbols();
    p->set_name(theEngine->booleanInputSymbols[i]->n);
    p->set_type(naothmessages::XABSLParameter_ParamType_Boolean);
    p->set_boolvalue(theEngine->booleanInputSymbols[i]->getValue());
  }//end for

  // enumeratedInputSymbols
  for (int i = 0; i < theEngine->enumeratedInputSymbols.getSize(); i++)
  {
    naothmessages::XABSLParameter* p = status.add_inputsymbols();
    p->set_name(theEngine->enumeratedInputSymbols[i]->n);
    p->set_type(naothmessages::XABSLParameter_ParamType_Enum);
    
    int value = theEngine->enumeratedInputSymbols[i]->getValue();
    p->set_enumvalue("-"); // TODO: output the number "value" as default

    // find the name for enum value
    for(int j = 0; j < theEngine->enumeratedInputSymbols[i]->enumeration->enumElements.getSize(); j++)
    {
      if(theEngine->enumeratedInputSymbols[i]->enumeration->enumElements[j]->v == value)
      {
        p->set_enumvalue(theEngine->enumeratedInputSymbols[i]->enumeration->enumElements[j]->n);
        break;
      }
    }//end for
  }//end for


  /*******************************************************************
   * output symbols
   *******************************************************************/

  // decimalOutputSymbols
  for (int i = 0; i < theEngine->decimalOutputSymbols.getSize(); i++)
  {
    naothmessages::XABSLParameter* p = status.add_outputsymbols();
    p->set_name(theEngine->decimalOutputSymbols[i]->n);
    p->set_type(naothmessages::XABSLParameter_ParamType_Decimal);
    p->set_decimalvalue(theEngine->decimalOutputSymbols[i]->getValue());
  }//end for

  // booleanOutputSymbols
  for (int i = 0; i < theEngine->booleanOutputSymbols.getSize(); i++)
  {
    naothmessages::XABSLParameter* p = status.add_outputsymbols();
    p->set_name(theEngine->booleanOutputSymbols[i]->n);
    p->set_type(naothmessages::XABSLParameter_ParamType_Boolean);
    p->set_boolvalue(theEngine->booleanOutputSymbols[i]->getValue());
  }//end for

  // enumeratedOutputSymbols
  for (int i = 0; i < theEngine->enumeratedOutputSymbols.getSize(); i++)
  {
    naothmessages::XABSLParameter* p = status.add_outputsymbols();
    p->set_name(theEngine->enumeratedOutputSymbols[i]->n);
    p->set_type(naothmessages::XABSLParameter_ParamType_Enum);

    int value = theEngine->enumeratedOutputSymbols[i]->getValue();
    p->set_enumvalue("-"); // TODO: output the number "value" as default

    // find the name for enum value
    for(int j = 0; j < theEngine->enumeratedOutputSymbols[i]->enumeration->enumElements.getSize(); j++)
    {
      if(theEngine->enumeratedOutputSymbols[i]->enumeration->enumElements[j]->v == value)
      {
        p->set_enumvalue(theEngine->enumeratedOutputSymbols[i]->enumeration->enumElements[j]->n);
        break;
      }
    }//end for
  }//end for

} // end fillRegisteredSymbols
