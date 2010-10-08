
#include "ModuleA.h"
#include "ModuleB.h"

void ModuleA::execute()
{
  cout << "start" << this->getModuleName() << endl;
  getRepresentationB().value = 3;

  ModuleCreator<ModuleB>* m = registerModule<ModuleB>("ModuleB");
  m->setEnabled(true);
  m->execute();

  getRepresentationB().value = 4;
  cout << "end" << this->getModuleName() << endl;
}//end execute



