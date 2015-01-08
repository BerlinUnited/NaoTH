/* 
 * File:   Simulator.h
 * Author: Heinrich
 *
 * Created on 4. April 2014, 16:51
 */

#ifndef _LogProviderModule_h_
#define _LogProviderModule_h_

#include <ModuleFramework/Module.h>
#include "LogFileScanner.h"

#include <Tools/Debug/DebugRequest.h>


BEGIN_DECLARE_MODULE(LogProvider)
  PROVIDE(DebugRequest)
END_DECLARE_MODULE(LogProvider)


class LogProvider: public LogProviderBase, virtual public BlackBoardInterface
{
private:
  const LogFileScanner::Frame* representations;
  std::set<std::string> exludeMap;
  LogFileScanner* scanner;

public:
  LogProvider() : representations(NULL) 
  {
    // HACK: do not provide basic percepts (they are provided by get(...))
    exludeMap.insert("AccelerometerData");
    exludeMap.insert("SensorJointData");
    exludeMap.insert("Image");
    exludeMap.insert("ImageTop");
    exludeMap.insert("FSRData");
    exludeMap.insert("GyrometerData");
    exludeMap.insert("InertialSensorData");
    exludeMap.insert("IRReceiveData");
    exludeMap.insert("CurrentCameraSettings");
    exludeMap.insert("ButtonData");
    exludeMap.insert("BatteryData");
    exludeMap.insert("UltraSoundReceiveData");
    exludeMap.insert("FrameInfo");
  }

  void init(LogFileScanner& logScanner, const LogFileScanner::Frame& rep, const std::set<std::string>& includedRepresentations)
  {
    scanner = &logScanner;
    representations = &rep;
    std::set<std::string>::iterator iter;

    for(iter = includedRepresentations.begin(); iter != includedRepresentations.end(); ++iter) {
      if(*iter != "") {
        DEBUG_REQUEST_REGISTER("LogProvider:"+(*iter), "", true);
      }
    }
    //DEBUG_REQUEST_REGISTER("FrameInfo", "", false);
  }//end init


  void execute()
  {
    BlackBoard& blackBoard = BlackBoardInterface::getBlackBoard();
    
    for(BlackBoard::Registry::iterator bbData = blackBoard.getRegistry().begin(); bbData != blackBoard.getRegistry().end(); ++bbData)
    {
      // look if there is a logged data for this representation 
      LogFileScanner::Frame::const_iterator frameData = representations->find(bbData->first); 
      if( frameData != representations->end() && 
          frameData->second.valid && 
          frameData->first != "" &&
          exludeMap.find(frameData->first) == exludeMap.end()
        )
      {
        DEBUG_REQUEST_GENERIC("LogProvider:"+(frameData->first),
          std::istrstream stream(frameData->second.data.data(), frameData->second.data.size());
          bbData->second->getRepresentation().deserialize(stream);
        );
      }
    }//end for
  }//end execute

  bool getRepresentation(const std::string& name, std::ostream& stream)
  {
    BlackBoard& blackBoard = BlackBoardInterface::getBlackBoard();
    BlackBoard::Registry::iterator bbData = blackBoard.getRegistry().find(name);

    if(bbData == blackBoard.getRegistry().end()) {
      return false;
    }

    bbData->second->getRepresentation().serialize(stream);

    return true;
  }
};

#endif  /* _LogProviderModule_h_ */


