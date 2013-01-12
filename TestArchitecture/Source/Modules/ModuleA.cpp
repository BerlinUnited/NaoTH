
#include "ModuleA.h"
#include "ModuleB.h"

using namespace std;

void ModuleA::execute()
{
  cout << "[" << getName() << "] " << "start" << endl;
  getRepresentationB().value = 3;

  ModuleCreator<ModuleB>* m = registerModule<ModuleB>("ModuleB");
  m->setEnabled(true);
  m->execute();

  getRepresentationB().value = 4;
  cout << "[" << getName() << "] " <<  "end" << endl;
}//end execute



