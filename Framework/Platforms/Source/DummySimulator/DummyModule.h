/*
* File:   DummyModule.h
* Author: schlottb[at]informatik.hu-berlin.de
*
* Created on 2017.05.21
*/
#ifndef _DummyModule_h_
#define _DummyModule_h_

#include <vector>

#include <ModuleFramework/Module.h>

#include <Tools/Debug/DebugRequest.h>

BEGIN_DECLARE_MODULE(DummyModule)
//  PROVIDE(DebugRequest)
END_DECLARE_MODULE(DummyModule)

class DummyModule : public DummyModuleBase, virtual public BlackBoardInterface
{
protected:
  inline DebugRequest& getDebugRequest() {
    return *(BlackBoardInterface::getBlackBoard().getRepresentation<DataHolder<DebugRequest> >("DebugRequest"));
  }

public:
  struct RepresentationData
  {
    RepresentationData() : valid(false) {}
    bool valid;
    std::vector<char> data;
  };
  //
  typedef std::map<std::string, RepresentationData > Frame;
  typedef std::list<unsigned int>::const_iterator FrameIterator;

  void readFrame(unsigned int currentFrame, Frame& frame);

  FrameIterator begin() {
    return frames.begin();
  }

  FrameIterator end() {
    return frames.end();
  }

  FrameIterator last() {
    return --frames.end();
  }

private:
  std::list<unsigned int> frames;

public:  
  void init()
  {

  }//end init


  void execute()
  {
    
  }//end execute

  bool getRepresentation(const std::string& name, std::ostream& stream)
  {

  }
};

#endif  /* _DummyModule_h_ */