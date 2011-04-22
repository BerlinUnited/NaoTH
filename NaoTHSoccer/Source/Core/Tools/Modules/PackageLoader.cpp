#include "PackageLoader.h"

#include <list>

#include <PlatformInterface/Platform.h>
#include <Representations/Infrastructure/Configuration.h>

PackageLoader::PackageLoader()
{
}

void PackageLoader::loadPackages(const std::string& directory)
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
          loadSinglePackage(completeFileName);
        }
      }
    }
    
    g_dir_close(packageDir);
  }
}

void PackageLoader::loadSinglePackage(const std::string& fileName)
{
  GModule* package = g_module_open(fileName.c_str(), G_MODULE_BIND_MASK);
  if(package != NULL)
  {
    // TODO: call package register function
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

