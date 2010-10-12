
#include "ModuleB.h"


void ModuleB::execute()
{
  cout << "start" << this->getModuleName() << endl;
  //cout << (theRepresentationB).value << endl;
  //cout << (theMyRep).c << endl;

  cout << getRepresentationB().value << endl;

  cout << "end" << this->getModuleName() << endl;
}//end execute
