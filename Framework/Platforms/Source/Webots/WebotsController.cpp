/**
 * @file WebotsController.cpp
 *
 * @author <a href="mailto:schlottb@informatik.hu-berlin.de">Schlotter, Benjamin</a>
 * @author <a href="mailto:mellmann@informatik.hu-berlin.de">Heinrich Mellmann</a>
 * @brief Interface for the Webots simulator
 *
 */

#include "WebotsController.h"

#include <iostream>
#include <fstream>

#include "PlatformInterface/Platform.h"
#include "Tools/Communication/MessageQueue/MessageQueue4Threads.h"

#include <Tools/ImageProcessing/ColorModelConversions.h>
#include <Tools/StringTools.h>
#include <Tools/ThreadUtil.h>
#include <Tools/NaoTime.h>
#include <Tools/NaoInfo.h>
#include <Tools/Math/Common.h>
#include <Tools/Communication/ASCIIEncoder.h>

using namespace std;

WebotsController::WebotsController(const std::string& name)
  : 
  thePlatformName(name),
  theSyncMode(false)
{
  // register input
  registerInput<FrameInfo>(*this);

  registerInput<SensorJointData>(*this);
  registerInput<AccelerometerData>(*this);
  registerInput<FSRData>(*this);
  registerInput<GyrometerData>(*this);
  registerInput<InertialSensorData>(*this);
  registerInput<BatteryData>(*this);

  //registerInput<Image>(*this);
  registerInput<VirtualVision>(*this);
  registerInput<VirtualVisionTop>(*this);
  registerInput<TeamMessageDataIn>(*this);
  registerInput<GameData>(*this);
  registerInput<GPSData>(*this);

  // register output
  registerOutput<const MotorJointData>(*this);
  registerOutput<const TeamMessageDataOut>(*this);

  // debug
  registerInput<DebugMessageInCognition>(*this);
  registerInput<DebugMessageInMotion>(*this);
  registerOutput<DebugMessageOut>(*this);


  // calculate debug communicaiton port
  //unsigned short debugPort = static_cast<short unsigned int> (5000 + (teamNumber*100) + playerNumber);
  theDebugServer.start(5401);
  theDebugServer.setTimeOut(0);


  GError *err = NULL;
  socket = g_socket_new(G_SOCKET_FAMILY_IPV4, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_TCP, &err);
  if (err)
  {
    socket = NULL;
    std::cout << "[WARN] Could not create a socket. This is a fatal error and communication is available. Error message:"
              << std::endl << err->message << std::endl;
    g_error_free (err);
  }
}

WebotsController::~WebotsController()
{
  if (socket != NULL) {
    g_socket_close(socket, NULL);
  }
}

bool WebotsController::connect(const std::string& host, int port)
{
  if(socket == NULL) {
    return false;
  }

  cout << "[WebotsController] connecting to " << host << ":" << port << endl;

  gboolean conn = false;
  GCancellable* cancellable = NULL;
  GSocketAddress* sockaddr = NULL;
  GError* conn_error = NULL;
  GError* error = NULL;

  GSocketConnectable* addr = g_network_address_new(host.c_str(),static_cast<guint16> (port));
  GSocketAddressEnumerator* enumerator = g_socket_connectable_enumerate(addr);
  g_object_unref(addr);

  /**
  * Try each sockaddr until we succeed. Record the first
  * connection error, but not any further ones (since they'll probably be basically the same as the first).
  */
  while (!conn && (sockaddr = g_socket_address_enumerator_next(enumerator, cancellable, &error)))
  {
    conn = g_socket_connect(socket, sockaddr, NULL, conn_error ? NULL : &conn_error);
    g_object_unref(sockaddr);
  }
  g_object_unref(enumerator);
  if (conn)
  {
    /**
    * We couldn't connect to the first address, but we succeeded
    * in connecting to a later address.
    */
    if (conn_error) {
      g_error_free (conn_error);
    }
    return true;
  }

  if(error) {
    std::cout  << "[WARN] Could not connect:" << std::endl << error->message << std::endl;
    g_error_free(error);
  }

  if (conn_error) {
    std::cout << "[WARN] Could not connect:" << std::endl << conn_error->message << std::endl;
    g_error_free(conn_error);
  }

  return false;
}//end connect


bool WebotsController::init(
  const std::string& modelPath, 
  const std::string& parameterTeamName, 
  unsigned int parameterTeamNumber, 
  unsigned int parameterPlayerNumber, 
  const std::string& server, 
  unsigned int port, 
  bool sync)
{
  // IMPORTANT: initialize the platform
  Platform::getInstance().init(this);


  Configuration& config = Platform::getInstance().theConfiguration;

  // set the team number first
  unsigned int teamNumber;
  if (parameterTeamNumber != 0) {
    teamNumber = parameterTeamNumber;
  } else if (config.hasKey("player", "TeamNumber")) {
    teamNumber = config.getInt("player", "TeamNumber");
  } else {
    teamNumber = 4;
  }

  // use the player number if one is given, otherwise use the number from the simulation
  unsigned int playerNumber;
  if(parameterPlayerNumber != 0) {
    playerNumber = parameterPlayerNumber;
  } else if(config.hasKey("player", "PlayerNumber") && config.getInt("player", "PlayerNumber") != 0) {
    playerNumber = config.getInt("player", "PlayerNumber");
  } else {
    playerNumber = 0;
  }

  // use the team name if one is given, otherwise generate a name
  string teamName;
  if (!parameterTeamName.empty()) {
    teamName = parameterTeamName;
  } else if (config.hasKey("player", "TeamName")) {
    teamName = config.getString("player", "TeamName");
  } else {
    teamName = "NaoTH-" + std::to_string(teamNumber);
  }

  theSyncMode = sync;

  // connect to the simulator
  if(!connect(server, port)) {
    std::cerr << "WebotsController could not connect" << std::endl;
    return false;
  }
  theSocket.init(socket);

  // TODO: send create command to simulator
  //"rsg/agent/naov4/nao.rsg";
  //theSocket << "(scene " << modelPath << ")" << send;
  header.model = modelPath;

  std::stringstream sbuf;
  msgpack::pack(sbuf, header);

  theSocket << sbuf.str() << send;

  // wait the response
  //getSensorData();
  std::cout << "Sensordata: " << receiveBuffer << std::endl;

  // initialize the teamname and number
  //theSocket << "(init (teamname " << theGameInfo.teamName << ")(unum " << theGameInfo.playerNumber << "))" << theSync << send;

  cout << "NaoTH Simpark initialization successful: " << teamName << " " << playerNumber << endl;

  // hack?
  config.setInt("player", "TeamNumber", teamNumber);
  config.setInt("player", "PlayerNumber", playerNumber);
  config.setString("player", "TeamName", teamName);

  return true;
}

void WebotsController::main()
{
  cout << "WebotsController Controller runs in single thread" << endl;
  while ( getSensorData() )
  {
    if(motionCount >= 3) {
      PlatformInterface::runCognition();
      motionCount = 0;
    }

    PlatformInterface::runMotion();
    motionCount++;
    sendCommands();
  }
}


void WebotsController::sendCommands()
{
  // send command
  try {
    std::stringstream sbuf;
    msgpack::pack(sbuf, lolaActuators);

    theSocket << sbuf.str() << send;
  }
  catch(std::runtime_error& exp)
  {
    cerr << "can not send data to server, because of "<<exp.what()<<endl;
  }
}

bool WebotsController::getSensorData()
{
  try {
    theSocket >> receiveBuffer;

    // Test the message pack parsing
    msgpack::object_handle oh = msgpack::unpack(receiveBuffer.data(), receiveBuffer.size());

    // deserialized object is valid during the msgpack::object_handle instance is alive.
    msgpack::object deserialized = oh.get();

    // msgpack::object supports ostream.
    //std::cout << deserialized << std::endl;

    deserialized.convert(webotsSensors);

    //LolaDataConverter::readSensorData(dcmSensorData, lolaActuators);
  }
  catch (std::runtime_error& exp)
  {
    cerr<<"can not get data from server, because of "<<exp.what()<<endl;
    return false;
  }
  return true;
}

void WebotsController::get(FrameInfo& data)
{
  //data.setTime(data.getTime() + 20);
  // use the simulation time of webots
  data.setTime(static_cast<unsigned int>(webotsSensors.Time*1000.0+0.5));
  data.setFrameNumber(data.getFrameNumber() + 1);
}

void WebotsController::get(SensorJointData& data)
{
  // copy data from the buffer
  if(sensorJointDataBuffer.isFull()) {
    data = sensorJointDataBuffer.first();
  }

  // read the new data into the delayed buffer
  sensorJointDataBuffer.add();
  LolaDataConverter::get(lolaSensors, sensorJointDataBuffer.last());

  // hack: stiffness is not communicated yet
  for(size_t i = 0; i < naoth::JointData::numOfJoint; ++i) {
    //data.position[i] = theLastMotorJointData.position[i];
    //data.stiffness[i] = theLastMotorJointData.stiffness[i];
    sensorJointDataBuffer.last().stiffness[i] = theLastMotorJointData.stiffness[i];
  }
}

void WebotsController::set(const MotorJointData& data)
{
  LolaDataConverter::set(lolaActuators, data);
  theLastMotorJointData = data;
}

MessageQueue* WebotsController::createMessageQueue(const std::string& /*name*/)
{
  if ( theSyncMode ) {
    return new MessageQueue();
  } else {
    return new MessageQueue4Threads();
  }
}
