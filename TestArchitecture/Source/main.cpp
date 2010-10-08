#include <iostream>

#include "TestModuleManager.h"

using namespace std;

int main( int argc, const char* argv[] )
{
  cout << "start test" << endl;
  TestModuleManager manager;
  manager.init();
  manager.main();
  cout << "end test" << endl;

  return 0;
}//end main