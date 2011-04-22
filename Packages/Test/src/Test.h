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

#include <Representations/Infrastructure/FrameInfo.h>
#include <Core/Representations/Infrastructure/ColorTable64.h>

}

using namespace naoth;

BEGIN_DECLARE_MODULE(Test)
  REQUIRE(FrameInfo)
  PROVIDE(ColorTable64)
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

