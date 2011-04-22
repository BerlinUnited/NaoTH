#ifndef MODULELOADER_H
#define MODULELOADER_H

#include <vector>
#include <string>

#include <glib.h>
#include <gmodule.h>


#include <ModuleFramework/ModuleManager.h>

class PackageLoader
{

public:
  PackageLoader();
  virtual ~PackageLoader();

  void loadPackages(const std::string& directory, ModuleManager& manager);
private:
  void loadSinglePackage(const std::string& fileName, ModuleManager& manager);

  std::vector<GModule*> modules;

};

#endif // MODULELOADER_H
