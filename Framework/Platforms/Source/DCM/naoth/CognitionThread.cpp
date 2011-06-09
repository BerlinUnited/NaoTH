
#include "CognitionThread.h"
#include "NaoController.h"

using namespace naoth;

CognitionThread::CognitionThread()
:stopping(false)
{
}

CognitionThread::~CognitionThread()
{

}

void CognitionThread::preExecute()
{
}

void CognitionThread::postExecute()
{
}

void *CognitionThread::execute()
{
  while(!stopping)
  {
    //execute Cognition
    NaoController::getInstance().callCognition();
    periodWait();
  }//end while

  return NULL;

}//end execute
