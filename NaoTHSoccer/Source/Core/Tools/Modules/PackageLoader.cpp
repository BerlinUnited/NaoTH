#include "PackageLoader.h"

#include <list>

#include <PlatformInterface/Platform.h>
#include <Representations/Infrastructure/Configuration.h>

PackageLoader::PackageLoader()
{
}

void PackageLoader::loadPackages(const std::string& directory, ModuleManager& manager)
{
  std::string packageLocation = directory;
  GDir* packageDir = g_dir_open(packageLocation.c_str(), 0, NULL);
  if(packageDir != NULL)
  {
    const gchar* name;
    while ((name = g_dir_read_name(packageDir)) != NULL)
    {
      if (g_str_has_suffix(name, G_MODULE_SUFFIX))
      {
        std::string completeFileName = packageLocation + name;
        if (g_file_test(completeFileName.c_str(), G_FILE_TEST_EXISTS)
          && g_file_test(completeFileName.c_str(), G_FILE_TEST_IS_REGULAR))
        {
          loadSinglePackage(completeFileName, manager);
        }
      }
    }
    
    g_dir_close(packageDir);
  }
}

void PackageLoader::loadSinglePackage(const std::string& fileName, ModuleManager& manager)
{
  GModule* package = g_module_open(fileName.c_str(), G_MODULE_BIND_LAZY);
  if(package != NULL)
  {
    // declare a register function with a modulemanager as argument
    void (*naoth_register_modules) (ModuleManager*) = NULL;
    
    if(g_module_symbol(package, "naoth_register_modules", (gpointer*) &naoth_register_modules) 
      && naoth_register_modules != NULL)
    {
      naoth_register_modules(&manager);
      g_message("loaded package %s", fileName.c_str());
    }
    else
    {
      g_warning("could not load package %s, symbol \"naoth_register_modules\" not found", fileName.c_str());
    }
  }
}

PackageLoader::~PackageLoader()
{
  for(unsigned int i=0; i < modules.size(); i++)
  {
    if(modules[i] != NULL)
    {
      g_module_close(modules[i]);
    }
  }
  
  modules.clear();
}

