#include <jni.h>
#include <stdio.h>
#include "LogSimulator.h"

#include "LogSimulator/Simulator.h"
#include <PlatformInterface/Platform.h>
#include <Tools/Debug/Stopwatch.h>
#include <ModuleFramework/ModuleManager.h>
#include <ModuleFramework/ModuleCreator.h>

Simulator* simulator = NULL;
Cognition* cognitionProcess = NULL;
Motion* motionProcess = NULL;
LogProvider* logProvider = NULL;

// kind of a HACK, needed by the logsimulator
extern ModuleManager* getModuleManager(Cognition* c);

JNIEXPORT void JNICALL Java_de_naoth_rc_LogSimulator_openLogFile(JNIEnv *env, jobject thisObj, jstring path)
{
  // Step 1: Convert the JNI String (jstring) into C-String (char*)
  const char* inCStr = (*env).GetStringUTFChars(path, NULL);
  if (NULL == inCStr) { 
    return;
  }

  if(simulator != NULL) 
  {
    simulator->open(std::string(inCStr));
  }
  else
  {
    g_type_init();
    simulator = new Simulator(std::string(inCStr), true, false);

    // init the platform
    Platform::getInstance().init(simulator);
  
    //init_agent(sim);
    cognitionProcess = createCognition();
    motionProcess = createMotion();

    // ACHTUNG: C-Cast (!)
    ModuleManager* theCognitionManager = getModuleManager(cognitionProcess);
    if(!theCognitionManager)
    {
      std::cerr << "ERROR: theCognition is not of type ModuleManager" << std::endl;
      return;
    }

    // register a module to provide all the logged data
    ModuleCreator<LogProvider>* theLogProvider = theCognitionManager->registerModule<LogProvider>(std::string("LogProvider"));

    // register processes
    (*simulator).registerCognition((naoth::Callable*)(cognitionProcess));
    (*simulator).registerMotion((naoth::Callable*)(motionProcess));
 
    theLogProvider->setEnabled(true);
    theLogProvider->getModuleT()->init((*simulator).logFileScanner, (*simulator).getRepresentations(), (*simulator).getIncludedRepresentations());
    logProvider = theLogProvider->getModuleT();

    simulator->init();
  }

  // Step 2: Perform its intended operations
  (*env).ReleaseStringUTFChars(path, inCStr);  // release resources
}

JNIEXPORT void JNICALL Java_de_naoth_rc_LogSimulator_stepForward(JNIEnv *env, jobject thisObj)
{
  if(simulator != NULL) {
    simulator->stepForward();
  }
}

JNIEXPORT void JNICALL Java_de_naoth_rc_LogSimulator_stepBack(JNIEnv *env, jobject thisObj)
{
  if(simulator != NULL) {
    simulator->stepBack();
  }
}

JNIEXPORT void JNICALL Java_de_naoth_rc_LogSimulator_jumpToBegin(JNIEnv *env, jobject thisObj)
{
  if(simulator != NULL) {
    simulator->jumpToBegin();
  }
}

JNIEXPORT void JNICALL Java_de_naoth_rc_LogSimulator_jumpToEnd(JNIEnv *env, jobject thisObj)
{
  if(simulator != NULL) {
    simulator->jumpToEnd();
  }
}

JNIEXPORT void JNICALL Java_de_naoth_rc_LogSimulator_jumpTo(JNIEnv *env, jobject thisObj, jint position)
{
  if(simulator != NULL) {
    simulator->jumpTo(position);
  }
}

JNIEXPORT jint JNICALL Java_de_naoth_rc_LogSimulator_getCurrentFrame(JNIEnv *env, jobject thisObj)
{
  if(simulator != NULL) {
    return simulator->getCurrentFrame();
  }
  return -1;
}

JNIEXPORT jint JNICALL Java_de_naoth_rc_LogSimulator_getMinFrame(JNIEnv *env, jobject thisObj)
{
  if(simulator != NULL) {
    return simulator->getMinFrame();
  }
  return -1;
}

JNIEXPORT jint JNICALL Java_de_naoth_rc_LogSimulator_getMaxFrame(JNIEnv *env, jobject thisObj)
{
  if(simulator != NULL) {
    return simulator->getMaxFrame();
  }
  return -1;
}


JNIEXPORT jbyteArray Java_de_naoth_rc_LogSimulator_getRepresentation(JNIEnv * env, jobject thisObj, jstring name)
{
  const char* inCStr = (*env).GetStringUTFChars(name, NULL);
  if (NULL == inCStr) { 
    return NULL;
  }

  std::stringstream stream;
  logProvider->getRepresentation(std::string(inCStr), stream);

  std::string data(stream.str());

  //jbyte *data;
  //int size;

  jbyteArray result=(*env).NewByteArray(data.size());
  (*env).SetByteArrayRegion(result, 0, data.size(), (jbyte*)data.c_str());

  return result;
}