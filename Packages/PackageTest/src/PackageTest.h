/* 
 * File:   PackageTest.h
 * Author: thomas
 *
 * Created on 11. November 2010, 18:32
 */

#ifndef PACKAGE_TEST_H
#define	PACKAGE_TEST_H

extern "C++"
{
#include <ModuleFramework/Module.h>

#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/ColorTable64.h>

}

using namespace naoth;

BEGIN_DECLARE_MODULE(PackageTest)
  REQUIRE(FrameInfo)
  PROVIDE(ColorTable64)
END_DECLARE_MODULE(PackageTest)

class PackageTest : public PackageTestBase
{
public:
  PackageTest();
  virtual ~PackageTest();

  virtual void execute();

private:
};

#endif	/* PACKAGE_TEST_H */

