
#include "ModuleB.h"

using namespace std;

void ModuleB::execute()
{
  cout << "[" << getName() << "] " << "start" << endl;
//cout << "[" << getName() << "] " << (theRepresentationB).value << endl;
//cout << "[" << getName() << "] " << (theMyRep).c << endl;
  cout << "[" << getName() << "] " << getRepresentationB().value << endl;
  cout << "[" << getName() << "] " << "end" << endl;
}//end execute
