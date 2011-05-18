/* 
 * File:   Configuration.h
 * Author: thomas
 *
 * Created on 8. November 2010, 12:47
 */

#ifndef CONFIGURATION_H
#define	CONFIGURATION_H

#include <string>
#include <list>

#include <glib.h>

namespace naoth
{
  class Configuration
  {
  public:
    Configuration();
    Configuration(const Configuration& orig);
    virtual ~Configuration();

    void loadFromDir(std::string dirlocation, std::string scheme, std::string id);
    void loadFile(std::string file, std::string groupName);
    void clear();

    void save(std::string dirlocation);
    void saveFile(std::string file, std::string group);

    /**
     * Get all group names.
     * @param group
     * @return
     */
    std::list<std::string> getGroups() const;

    /**
     * Get all keys for a specific group.
     * @param group
     * @return
     */
    std::list<std::string> getKeys(std::string group) const;

    /**
     * Returns true if the configuration has a key with this name and group
     * @param group
     * @param key
     * @return
     */
    bool hasKey(std::string group, std::string key) const;

    /**
     * Get a string value from the configuration
     * @param group
     * @param key
     * @return
     */
    std::string getString(std::string group, std::string key) const;

    void setString(std::string group, std::string key, std::string value);

    /**
     * Get a double value from the configuration
     * @param group
     * @param key
     * @return
     */
    double getDouble(std::string group, std::string key) const;

    void setDouble(std::string group, std::string key, double value);

    /**
     * Get an integer value from the configuration
     * @param group
     * @param key
     * @return
     */
    int getInt(std::string group, std::string key) const;

    void setInt(std::string group, std::string key, int value);

    /**
     * Get a bool value from the configuration
     * @param group
     * @param key
     * @return
     */
    bool getBool(std::string group, std::string key) const;

    void setBool(std::string group, std::string key, bool value);

    /**
     * Get a raw and uninterpretated string from the configuration
     * @param group
     * @param key
     * @return
     */
    std::string getRawValue(std::string group, std::string key) const;

    void setRawValue(std::string group, std::string key, std::string value);

  private:

    GKeyFile* keyFile;

    void loadFromSingleDir(std::string dirlocation);

  };
}

#endif	/* CONFIGURATION_H */

