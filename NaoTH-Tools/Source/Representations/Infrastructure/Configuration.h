/* 
 * File:   Configuration.h
 * Author: thomas
 *
 * Created on 8. November 2010, 12:47
 */

#ifndef CONFIGURATION_H
#define	CONFIGURATION_H

#include <string>

#include <glib.h>

class Configuration
{
public:
  Configuration();
  Configuration(const Configuration& orig);
  virtual ~Configuration();

  void loadFromDir(std::string dirlocation, std::string platformName, std::string macAddress);

private:

  GKeyFile* keyFile;

};

#endif	/* CONFIGURATION_H */

