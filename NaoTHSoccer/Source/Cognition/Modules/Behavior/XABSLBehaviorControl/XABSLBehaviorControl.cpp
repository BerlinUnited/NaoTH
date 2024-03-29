/* 
 * File:   XABSLBehaviorControl.cpp
 * Author: thomas
 * 
 * Created on 9. Januar 2009, 21:31
 */

#include "XABSLBehaviorControl.h"

#include "Tools/Xabsl/EngineFactory.h"
#include <DebugCommunication/DebugCommandManager.h>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <PlatformInterface/Platform.h>

using namespace std;

XABSLBehaviorControl::XABSLBehaviorControl()
  : 
    // init the xabxl engine
    theEngine(NULL),
    theErrorHandler(error_stream),
    agentName("soccer_agent")
{
  REGISTER_DEBUG_COMMAND("behavior:reload", "reload the behavior file", this);
  REGISTER_DEBUG_COMMAND("behavior:get_agent", "get the current selected agent", this);
  REGISTER_DEBUG_COMMAND("behavior:set_agent", "behavior:set_agent agent=<agent_name>", this);
  REGISTER_DEBUG_COMMAND("behavior:list_agents", "return the list of all avaliable agents", this);
  REGISTER_DEBUG_COMMAND("behavior:get_registered_symbols", "return the list of registered symbols", this);
  REGISTER_DEBUG_COMMAND("behavior:get_symbol",
    "return the value of a symbol. usage behavior:get_symbol <symbol_name>", this);

  DEBUG_REQUEST_REGISTER("XABSL:draw_motion_request", "draw the motion intention of the robot on the field", false);


#define REGISTER_MODULE(module, enable) \
  the##module = registerModule<module>(std::string(#module), enable)

  // init symbols
  REGISTER_MODULE(BallSymbols, true);
  REGISTER_MODULE(GameSymbols, true);
  REGISTER_MODULE(MotionSymbols, true);
  REGISTER_MODULE(TeamSymbols, true);
  REGISTER_MODULE(SensorSymbols, true);
  REGISTER_MODULE(MathSymbols, true);
  REGISTER_MODULE(LedSymbols, true);
  REGISTER_MODULE(SelflocSymbols, true);
  REGISTER_MODULE(OdometrySymbols, true);
  REGISTER_MODULE(FieldSymbols, true);
  REGISTER_MODULE(StrategySymbols, true);
  REGISTER_MODULE(SoundSymbols, true);
  REGISTER_MODULE(PathSymbols, true);
  REGISTER_MODULE(RemoteSymbols, true);
  

  // load the behavior from config
  if(naoth::Platform::getInstance().theConfiguration.hasKey("agent", "agent")) {
    agentName = naoth::Platform::getInstance().theConfiguration.getString("agent", "agent");
  } else {
    std::cerr << "[XABSLBehaviorControl] ERROR: could not load \"Config/behavior/agent.cfg\"" << std::endl;
  }

  const string behaviorPath = naoth::Platform::getInstance().theConfigDirectory + "/behavior-ic.dat";
  loadBehaviorFromFile(behaviorPath, agentName);

}//end constructor

XABSLBehaviorControl::~XABSLBehaviorControl()
{
  delete theEngine;
}


void XABSLBehaviorControl::loadBehaviorFromFile(std::string file, std::string agent)
{
  // clear old status
  getBehaviorStateComplete().state.Clear();

  // clear old errors
  error_stream.str("");
  error_stream.clear();

  // reset the error handler
  theErrorHandler.errorsOccurred = false;
  
  // Delete and creata new engine every time when a new behavior file is loaded. Just in case :)
  // delete the old engine if avaliable
  delete theEngine;
  // create a new engine 
  theEngine = xabsl::EngineFactory<XABSLBehaviorControl>::create(theErrorHandler, getFrameInfo());
  //
  registerXABSLSymbols();


  // load the behavior file
  XabslFileInputSource input(file);
  theEngine->createOptionGraph(input);
 
  // set the currently active agent
  if(!theEngine->setSelectedAgent(agent.c_str())) {
    std::cerr << "[XABSLBehaviorControl] ERROR: could not set the agent \"" << agent << "\" for behavior " << std::endl;
  }
  std::cout << "[XABSLBehaviorControl] current agent is set to \"" << theEngine->getSelectedAgentName() << "\"" << std::endl;
  agentName = theEngine->getSelectedAgentName();

  if(!theErrorHandler.errorsOccurred) 
  {
    inputDecimalBuffer.clear();
    inputBooleanBuffer.clear();
    inputEnumBuffer.clear();
    outputDecimalBuffer.clear();
    outputBooleanBuffer.clear();
    outputEnumBuffer.clear();

    fillRegisteredBehavior(getBehaviorStateComplete().state);
    getBehaviorStateComplete().state.set_framenumber(getFrameInfo().getFrameNumber());
    std::cout << "[XABSLBehaviorControl] " << getBehaviorStateComplete().state.IsInitialized() << " - " << getBehaviorStateComplete().state.framenumber() << "\n";
  }
}//end reloadBehaviorFromFile


void XABSLBehaviorControl::execute()
{
  // execute the behavior
  if(theEngine != NULL && !theErrorHandler.errorsOccurred)
  {
    updateXABSLSymbols();
    theEngine->execute();
    updateOutputSymbols();
    //cout << "test " << getMotionStatus().motionRequest << endl;

    // TODO: only update sparse behavior status on some kind of request (or make it configurable)
    // update the behavior status
    STOPWATCH_START("XABSLBehaviorControl:fillBehaviorStateSparse");
    getBehaviorStateSparse().state.Clear();
    fillActiveOptionsSparse(getBehaviorStateSparse().state);
    fillRegisteredSymbolsSparserer(getBehaviorStateSparse().state);
    getBehaviorStateSparse().state.set_framenumber(getFrameInfo().getFrameNumber());
    STOPWATCH_STOP("XABSLBehaviorControl:fillBehaviorStateSparse");

    draw();
  }//end if

  if(theErrorHandler.errorsOccurred)
  {
    getBehaviorStateSparse().state.set_errormessage(error_stream.str());
  }
}//end execute


void XABSLBehaviorControl::draw()
{

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
        Pose2D robotPosePlanned = getRobotPose() + getMotionStatus().plannedMotion.hip;
        Pose2D walkRequest = getMotionRequest().walkRequest.target;
        walkRequest = robotPosePlanned+walkRequest;

        Vector2d target(100,0);
        target = walkRequest*target;

        ARROW(walkRequest.translation.x, walkRequest.translation.y, 
              target.x, target.y);
        TEXT_DRAWING(
             walkRequest.translation.x - 100,
             walkRequest.translation.y - 100,
             "WALK");
      }
      break;
      default: break;
    }//end switch
  );
}//end draw


#define XABSL_REGISTER_SYMBOLS(module) \
  the##module->getModuleT()->registerSymbols(*theEngine)

void XABSLBehaviorControl::registerXABSLSymbols()
{
  if(theEngine != NULL)
  {
    XABSL_REGISTER_SYMBOLS(BallSymbols);
    XABSL_REGISTER_SYMBOLS(GameSymbols);
    XABSL_REGISTER_SYMBOLS(MotionSymbols);
    XABSL_REGISTER_SYMBOLS(TeamSymbols);
    XABSL_REGISTER_SYMBOLS(SensorSymbols);
    XABSL_REGISTER_SYMBOLS(MathSymbols);
    XABSL_REGISTER_SYMBOLS(LedSymbols);
    XABSL_REGISTER_SYMBOLS(SelflocSymbols);
    XABSL_REGISTER_SYMBOLS(OdometrySymbols);
    XABSL_REGISTER_SYMBOLS(FieldSymbols);
    XABSL_REGISTER_SYMBOLS(StrategySymbols);
    XABSL_REGISTER_SYMBOLS(SoundSymbols);
    XABSL_REGISTER_SYMBOLS(PathSymbols);
    XABSL_REGISTER_SYMBOLS(RemoteSymbols);
  }
}//end registerXABSLSymbols


void XABSLBehaviorControl::updateXABSLSymbols()
{
  if(theEngine != NULL)
  {
    theSelflocSymbols->execute();
    theSensorSymbols->execute();
    theBallSymbols->execute();
    theOdometrySymbols->execute();
    theMotionSymbols->execute();
    theStrategySymbols->execute();
    // right now, not doing anything
    thePathSymbols->execute();
  }
}//end updateXABSLSymbols

void XABSLBehaviorControl::updateOutputSymbols()
{
  if(theEngine != NULL)
  {
    theMotionSymbols->getModuleT()->updateOutputSymbols();
  }
}//end updateOutputSymbols


// ERROR HANDLER //

MyErrorHandler::MyErrorHandler(std::ostream& out)
  : out(out)
{
}

void MyErrorHandler::printError(const char* txt)
{
  out << "XABSL error: " << txt << std::endl;
  std::cerr << "XABSL error: " << txt << std::endl;
}

void MyErrorHandler::printMessage(const char* txt)
{
  out << "XABSL message: " << txt << std::endl;
  std::cout << "XABSL message: " << txt << std::endl;
}

MyErrorHandler::~MyErrorHandler()
{
}


void XABSLBehaviorControl::executeDebugCommand(
    const std::string& command, 
    const std::map<std::string, std::string>& arguments,
    std::ostream & outstream)
{
  if(command == "behavior:reload")
  {
    // restart the behavior
    const string behaviorPath = naoth::Platform::getInstance().theConfigDirectory + "/behavior-ic.dat";
    loadBehaviorFromFile(behaviorPath, agentName);
    
    if(theErrorHandler.errorsOccurred)
      outstream << error_stream.str();
    else
      outstream << "behavior reloaded";
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

void XABSLBehaviorControl::fillActiveOptionsSparse(naothmessages::BehaviorStateSparse &status)
{
  const xabsl::Array<xabsl::Action*>& actions = theEngine->getRootActions();
  for (int i=0; i < actions.getSize(); i++)
  {
    naothmessages::XABSLActionSparse* action = status.add_activerootactions();
    fillActionSparse(actions[i], action);
  }
}

void XABSLBehaviorControl::fillActionSparse(const xabsl::Action* source, naothmessages::XABSLActionSparse* dest)
{
  if (const xabsl::Behavior* behavior = source->getBehavior())
  {
    const xabsl::Option* option = source->getOption();
    const xabsl::ParameterAssignment* parameters = source->getParameters();
    naothmessages::XABSLActiveOptionSparse* option_msg = dest->mutable_option();
    option_msg->set_timeofexecution(behavior->timeOfExecution);

    if (option) {
      dest->set_type(naothmessages::XABSLActionSparse_ActionType_Option);
      option_msg->set_id(option->index);
      option_msg->set_activestate(option->activeState->index);
      option_msg->set_statetime(option->activeState->timeOfStateExecution);
    } else {
      dest->set_type(naothmessages::XABSLActionSparse_ActionType_BasicBehavior);
      option_msg->set_id(source->getBasicBehavior()->index);
    }

    for (int j = 0; j < parameters->decimalValues.getSize(); j++)
    {
      option_msg->add_decimalparameters(parameters->decimalValues[j]);
    }
    for (int j = 0; j < parameters->booleanValues.getSize(); j++)
    {
      option_msg->add_booleanparameters(parameters->booleanValues[j]);
    }
    for (int j = 0; j < parameters->enumeratedValues.getSize(); j++)
    {
      option_msg->add_enumeratedparameters(parameters->enumeratedValues[j]);
    }

    // stream out children options
    const xabsl::Array<xabsl::Action*>& actions = option->activeState->actions;
    for(int j=0; j < actions.getSize(); j++)
    {
      fillActionSparse(actions[j], option_msg->add_activesubactions());
    }
  }
  else // it's a symbol
  {
    dest->set_type(naothmessages::XABSLActionSparse_ActionType_SymbolAssignment);
    naothmessages::XABSLSymbol* symbol_msg = dest->mutable_symbol();

    if(const xabsl::DecimalOutputSymbol* symbol = source->getDecimalOutputSymbol())
    {
      symbol_msg->set_type(naothmessages::XABSLSymbol_SymbolType_Decimal);
      symbol_msg->set_id(symbol->index);
      symbol_msg->set_decimalvalue(symbol->getValue());
    }
    else if(const xabsl::BooleanOutputSymbol* symbol = source->getBooleanOutputSymbol())
    {
      symbol_msg->set_type(naothmessages::XABSLSymbol_SymbolType_Boolean);
      symbol_msg->set_id(symbol->index);
      symbol_msg->set_boolvalue(symbol->getValue());
    }
    else if(const xabsl::EnumeratedOutputSymbol* symbol = source->getEnumeratedOutputSymbol())
    {
      symbol_msg->set_type(naothmessages::XABSLSymbol_SymbolType_Enum);
      symbol_msg->set_id(symbol->index);
      symbol_msg->set_enumvalue(symbol->getValue());
      symbol_msg->set_enumtypeid(symbol->enumeration->index);
    }
  }
} // end fillAction

void XABSLBehaviorControl::fillRegisteredBehavior(naothmessages::BehaviorStateComplete &status)
{
  for (int i = 0; i < theEngine->getAgents().getSize(); i++)
  {
    naothmessages::BehaviorStateComplete_Agent *agent_msg = status.add_agents();
    agent_msg->set_name(theEngine->getAgents()[i]->n);
    agent_msg->set_rootoption(theEngine->getAgents()[i]->getRootOption()->n);
  }

  for (int i = 0; i < theEngine->getOptions().getSize(); i++)
  {
    naothmessages::BehaviorStateComplete_Option* option_msg = status.add_options();
    const xabsl::Option* option = theEngine->getOptions()[i];

    option_msg->set_name(option->n);

    for(int j = 0; j < option->states.getSize(); j++)
    {
      naothmessages::BehaviorStateComplete_Option_State* state_msg = option_msg->add_states();
      state_msg->set_name(option->states[j]->n);
      state_msg->set_target(option->states[j]->isTargetState());
    }

    // decimal parameters
    for(int j = 0; j < option->parameters->decimal.getSize(); j++)
    {
      naothmessages::XABSLSymbol* p = option_msg->add_parameters();
      p->set_type(naothmessages::XABSLSymbol_SymbolType_Decimal);
      p->set_name(option->parameters->decimal.getName(j));
      p->set_id(j);
      p->set_decimalvalue(*(option->parameters->decimal.getElement(j)));
    }

    // boolean parameters
    for(int j = 0; j < option->parameters->boolean.getSize(); j++)
    {
      naothmessages::XABSLSymbol* p = option_msg->add_parameters();
      p->set_type(naothmessages::XABSLSymbol_SymbolType_Boolean);
      p->set_name(option->parameters->boolean.getName(j));
      p->set_id(j);
      p->set_boolvalue(*(option->parameters->boolean.getElement(j)));
    }

    // enum parameters
    assert(option->parameters->enumerated.getSize() == option->parameters->enumerations.getSize());
    for(int j = 0; j < option->parameters->enumerated.getSize(); j++)
    {
      naothmessages::XABSLSymbol* p = option_msg->add_parameters();
      p->set_type(naothmessages::XABSLSymbol_SymbolType_Enum);
      p->set_name(option->parameters->enumerated.getName(j));
      p->set_id(j);
      p->set_enumvalue(*(option->parameters->enumerated.getElement(j)));
      p->set_enumtypeid(option->parameters->enumerations.getElement(j)->index);
    }
  }

  for (int i = 0; i < theEngine->enumerations.getSize(); i++)
  {
    naothmessages::BehaviorStateComplete_EnumType* enum_msg = status.add_enumerations();
    const xabsl::Enumeration& enumeration = *(theEngine->enumerations[i]);

    enum_msg->set_name(enumeration.n);
    for(int j = 0; j < enumeration.enumElements.getSize(); j++)
    {
      naothmessages::BehaviorStateComplete_EnumType_Element* element = enum_msg->add_elements();
      element->set_value(enumeration.enumElements[j]->v);
      element->set_name(enumeration.enumElements[j]->n);
    }
  }

  /*******************************************************************
   * input symbols
   *******************************************************************/

  // decimalInputSymbols
  for (int i = 0; i < theEngine->decimalInputSymbols.getSize(); i++)
  {
    naothmessages::SymbolValueList_DoubleSymbol* p = status.mutable_inputsymbollist()->add_decimal();
    p->set_name(theEngine->decimalInputSymbols[i]->n);
    p->set_id(theEngine->decimalInputSymbols[i]->index);
    p->set_value(theEngine->decimalInputSymbols[i]->getValue());

    inputDecimalBuffer.push_back(theEngine->decimalInputSymbols[i]->getValue());
  }

  // booleanInputSymbols
  for (int i = 0; i < theEngine->booleanInputSymbols.getSize(); i++)
  {
    naothmessages::SymbolValueList_BooleanSymbol* p = status.mutable_inputsymbollist()->add_boolean();
    p->set_name(theEngine->booleanInputSymbols[i]->n);
    p->set_id(theEngine->booleanInputSymbols[i]->index);
    p->set_value(theEngine->booleanInputSymbols[i]->getValue());

    inputBooleanBuffer.push_back(theEngine->booleanInputSymbols[i]->getValue());
  }

  // enumeratedInputSymbols
  for (int i = 0; i < theEngine->enumeratedInputSymbols.getSize(); i++)
  {
    naothmessages::SymbolValueList_EnumSymbol* p = status.mutable_inputsymbollist()->add_enumerated();
    p->set_id(theEngine->enumeratedInputSymbols[i]->index);
    p->set_value(theEngine->enumeratedInputSymbols[i]->getValue());
    p->set_name(theEngine->enumeratedInputSymbols[i]->n);
    p->set_typeid_(theEngine->enumeratedInputSymbols[i]->enumeration->index);

    inputEnumBuffer.push_back(theEngine->enumeratedInputSymbols[i]->getValue());
  }

  /*******************************************************************
   * output symbols
   *******************************************************************/

  // decimalOutputSymbols
  for (int i = 0; i < theEngine->decimalOutputSymbols.getSize(); i++)
  {
    naothmessages::SymbolValueList_DoubleSymbol* p = status.mutable_outputsymbollist()->add_decimal();
    p->set_id(theEngine->decimalOutputSymbols[i]->index);
    p->set_name(theEngine->decimalOutputSymbols[i]->n);
    p->set_value(theEngine->decimalOutputSymbols[i]->getValue());

    outputDecimalBuffer.push_back(theEngine->decimalOutputSymbols[i]->getValue());
  }

  // booleanOutputSymbols
  for (int i = 0; i < theEngine->booleanOutputSymbols.getSize(); i++)
  {
    naothmessages::SymbolValueList_BooleanSymbol* p = status.mutable_outputsymbollist()->add_boolean();
    p->set_id(theEngine->booleanOutputSymbols[i]->index);
    p->set_name(theEngine->booleanOutputSymbols[i]->n);
    p->set_value(theEngine->booleanOutputSymbols[i]->getValue());

    outputBooleanBuffer.push_back(theEngine->booleanOutputSymbols[i]->getValue());
  }

  // enumeratedOutputSymbols
  for (int i = 0; i < theEngine->enumeratedOutputSymbols.getSize(); i++)
  {
    naothmessages::SymbolValueList_EnumSymbol* p = status.mutable_outputsymbollist()->add_enumerated();
    p->set_id(theEngine->enumeratedOutputSymbols[i]->index);
    p->set_value(theEngine->enumeratedOutputSymbols[i]->getValue());
    p->set_typeid_(theEngine->enumeratedOutputSymbols[i]->enumeration->index);
    p->set_name(theEngine->enumeratedOutputSymbols[i]->n);

    outputEnumBuffer.push_back(theEngine->enumeratedOutputSymbols[i]->getValue());
  }
}


void XABSLBehaviorControl::fillRegisteredSymbolsSparserer(naothmessages::BehaviorStateSparse &status)
{
 /*******************************************************************
   * input symbols
   *******************************************************************/

  // decimalInputSymbols
  for (int i = 0; i < theEngine->decimalInputSymbols.getSize(); i++)
  {
    if(inputDecimalBuffer[static_cast<unsigned int>(i)] != theEngine->decimalInputSymbols[i]->getValue())
    {
      naothmessages::SymbolValueList_DoubleSymbol* s = status.mutable_inputsymbollist()->add_decimal();
      s->set_value(theEngine->decimalInputSymbols[i]->getValue());
      s->set_id(i);
    }
  }

  // booleanInputSymbols
  for (int i = 0; i < theEngine->booleanInputSymbols.getSize(); i++)
  {
    if(inputBooleanBuffer[static_cast<unsigned int>(i)] != theEngine->booleanInputSymbols[i]->getValue())
    {
      naothmessages::SymbolValueList_BooleanSymbol* s = status.mutable_inputsymbollist()->add_boolean();
      s->set_value(theEngine->booleanInputSymbols[i]->getValue());
      s->set_id(i);
    }
  }

  // enumeratedInputSymbols
  for (int i = 0; i < theEngine->enumeratedInputSymbols.getSize(); i++)
  {
    if(inputEnumBuffer[static_cast<unsigned int>(i)] != theEngine->enumeratedInputSymbols[i]->getValue())
    {
      naothmessages::SymbolValueList_EnumSymbol* s = status.mutable_inputsymbollist()->add_enumerated();
      s->set_value(theEngine->enumeratedInputSymbols[i]->getValue());
      s->set_id(i);
    }
  }

  /*******************************************************************
   * output symbols
   *******************************************************************/

  // decimalInputSymbols
  for (int i = 0; i < theEngine->decimalOutputSymbols.getSize(); i++)
  {
    if(outputDecimalBuffer[static_cast<unsigned int>(i)] != theEngine->decimalOutputSymbols[i]->getValue())
    {
      naothmessages::SymbolValueList_DoubleSymbol* s = status.mutable_outputsymbollist()->add_decimal();
      s->set_value(theEngine->decimalOutputSymbols[i]->getValue());
      s->set_id(i);
    }
  }

  // booleanInputSymbols
  for (int i = 0; i < theEngine->booleanOutputSymbols.getSize(); i++)
  {
    if(outputBooleanBuffer[static_cast<unsigned int>(i)] != theEngine->booleanOutputSymbols[i]->getValue())
    {
      naothmessages::SymbolValueList_BooleanSymbol* s = status.mutable_outputsymbollist()->add_boolean();
      s->set_value(theEngine->booleanOutputSymbols[i]->getValue());
      s->set_id(i);
    }
  }

  // enumeratedInputSymbols
  for (int i = 0; i < theEngine->enumeratedOutputSymbols.getSize(); i++)
  {
    if(outputEnumBuffer[static_cast<unsigned int>(i)] != theEngine->enumeratedOutputSymbols[i]->getValue())
    {
      naothmessages::SymbolValueList_EnumSymbol* s = status.mutable_outputsymbollist()->add_enumerated();
      s->set_value(theEngine->enumeratedOutputSymbols[i]->getValue());
      s->set_id(i);
    }
  }
}


void XABSLBehaviorControl::fillRegisteredSymbolsSparse(naothmessages::BehaviorStateSparse &status)
{
 /*******************************************************************
   * input symbols
   *******************************************************************/

  // decimalInputSymbols
  naothmessages::SymbolValueList* input = status.mutable_inputsymbollist();
  for (int i = 0; i < theEngine->decimalInputSymbols.getSize(); i++)
  {
      naothmessages::SymbolValueList_DoubleSymbol* p = input->add_decimal();
      //p->set_name(theEngine->decimalInputSymbols[i]->n);
      p->set_id(theEngine->decimalInputSymbols[i]->index);
      p->set_value(theEngine->decimalInputSymbols[i]->getValue());
  }

  // booleanInputSymbols
  for (int i = 0; i < theEngine->booleanInputSymbols.getSize(); i++)
  {
    naothmessages::SymbolValueList_BooleanSymbol* p = input->add_boolean();
    //p->set_name(theEngine->booleanInputSymbols[i]->n);
    p->set_id(theEngine->booleanInputSymbols[i]->index);
    p->set_value(theEngine->booleanInputSymbols[i]->getValue());
  }

  // enumeratedInputSymbols
  for (int i = 0; i < theEngine->enumeratedInputSymbols.getSize(); i++)
  {
    naothmessages::SymbolValueList_EnumSymbol* p = input->add_enumerated();
    p->set_id(theEngine->enumeratedInputSymbols[i]->index);
    p->set_value(theEngine->enumeratedInputSymbols[i]->getValue());
    //p->set_enumtypeid(theEngine->enumeratedInputSymbols[i]->enumeration->index);
  }

  /*******************************************************************
   * output symbols
   *******************************************************************/

  naothmessages::SymbolValueList* output = status.mutable_outputsymbollist();
  // decimalOutputSymbols
  for (int i = 0; i < theEngine->decimalOutputSymbols.getSize(); i++)
  {
    naothmessages::SymbolValueList_DoubleSymbol* p = output->add_decimal();
    p->set_id(theEngine->decimalOutputSymbols[i]->index);
    //p->set_name(theEngine->decimalOutputSymbols[i]->n);
    p->set_value(theEngine->decimalOutputSymbols[i]->getValue());
  }

  // booleanOutputSymbols
  for (int i = 0; i < theEngine->booleanOutputSymbols.getSize(); i++)
  {
    naothmessages::SymbolValueList_BooleanSymbol* p = output->add_boolean();
    p->set_id(theEngine->booleanOutputSymbols[i]->index);
    //p->set_name(theEngine->booleanOutputSymbols[i]->n);
    p->set_value(theEngine->booleanOutputSymbols[i]->getValue());
  }

  // enumeratedOutputSymbols
  for (int i = 0; i < theEngine->enumeratedOutputSymbols.getSize(); i++)
  {
    naothmessages::SymbolValueList_EnumSymbol* p = output->add_enumerated();
    p->set_id(theEngine->enumeratedOutputSymbols[i]->index);
    p->set_value(theEngine->enumeratedOutputSymbols[i]->getValue());
    //p->set_enumtypeid(theEngine->enumeratedOutputSymbols[i]->enumeration->index);
  }
}

