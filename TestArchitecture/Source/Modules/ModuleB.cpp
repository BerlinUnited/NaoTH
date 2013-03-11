
#include "ModuleB.h"

using namespace std;

#define M_PRINT(x) std::cout << "[" << getName() << "] " << x << std::endl

void ModuleB::execute()
{
  M_PRINT("start");
  M_PRINT("RepresentationB = " << getRepresentationB().value);
  M_PRINT("end");
}//end execute
