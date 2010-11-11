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
  void loadFile(std::string file, std::string groupName);
  void clear();

  /**
   * Returns true if the configuration has a key with this name and group
   * @param group
   * @param key
   * @return
   */
  bool hasKey(std::string group, std::string key);

  /**
   * Get a string value from the configuration
   * @param group
   * @param key
   * @return
   */
  std::string getString(std::string group, std::string key);

  void setString(std::string group, std::string key, std::string value);

private:

  GKeyFile* keyFile;

  void loadFromSingleDir(std::string dirlocation);

};

#endif	/* CONFIGURATION_H */

