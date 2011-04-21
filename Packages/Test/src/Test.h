/* 
 * File:   Test.h
 * Author: thomas
 *
 * Created on 11. November 2010, 18:32
 */

#ifndef TEST_H
#define	TEST_H

extern "C++"
{
#include <ModuleFramework/Module.h>

#include "Core/Representations/FieldInfo.h"
#include <Representations/Infrastructure/GyrometerData.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/Image.h>
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Infrastructure/InertialSensorData.h>
#include <Representations/Infrastructure/AccelerometerData.h>
#include <Representations/Infrastructure/FSRData.h>

}

using namespace naoth;

BEGIN_DECLARE_MODULE(Test)
  REQUIRE(FrameInfo)
  
END_DECLARE_MODULE(Test)

class Test : public TestBase
{
public:
  Test();
  virtual ~Test();

  virtual void execute();

private:
};

#endif	/* TEST_H */

