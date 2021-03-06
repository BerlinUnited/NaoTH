/*
 * @file Configuration.h
 *
 * @author <a href="mailto:krause@informatik.hu-berlin.de">Thomas Krause</a>
 * @author <a href="mailto:xu@informatik.hu-berlin.de">Xu Yuan</a>
 * @breief the gloabl configuration for NaoTH framework
 *
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <set>

#include <glib.h>

namespace naoth
{
class Configuration
{
public:
  Configuration();
  Configuration(const Configuration& orig);
  virtual ~Configuration();

  void loadFromDir(std::string dirlocation,
                   const std::string &platform,
                   const std::string& scheme,
                   const std::string &strategy,
                   const std::string& bodyID,
                   const std::string& headID,
                   const std::string& robotName);

  // note this function only save private keys
  void save();

  /**
    * Get all keys for a specific group.
    * @param group
    * @return
    */
  std::set<std::string> getKeys(const std::string& group) const;

  /**
    * Returns true if the configuration has a key with this name and group
    * @param group
    * @param key
    * @return
    */
  bool hasKey(const std::string& group, const std::string& key) const;

  /**
   * Returns true if the configuration has a group with this name
   * @param group
   */
  bool hasGroup(const std::string& group) const;


  /**
  * Returns true if the configuration has a group with this name
  * and copies the content of the key to value
  * @param group
  * @param key
  * @return
  */
  // EXPERIMENTAL
  bool getT(const std::string& group, const std::string& key, int& value)
  {
    if(hasKey(group,key)) { 
      value = getInt(group,key); 
      return true; 
    }
    return false;
  }//end get int


  /**
    * Get a string value from the configuration
    * @param group
    * @param key
    * @return
    */
  std::string getString(const std::string& group, const std::string& key) const;
  
  bool get(const std::string& group, const std::string& key, std::string& result) const {
    if(hasKey(group,key)) { 
      result = getString(group, key);
      return true; 
    }
    return false;
  }

  void setString(const std::string& group, const std::string& key, const std::string& value);
  void set(const std::string& group, const std::string& key, const std::string& value) {
    setString(group, key, value);
  }
  void setDefault(const std::string& group, const std::string& key, const std::string& value);

  /**
    * Get a double value from the configuration
    * @param group
    * @param key
    * @return
    */
  double getDouble(const std::string& group, const std::string& key) const;
  bool get(const std::string& group, const std::string& key, double& result) const {
    if(hasKey(group,key)) { 
      result = getDouble(group, key);
      return true; 
    }
    return false;
  }

  void setDouble(const std::string& group, const std::string& key, double value);
  void set(const std::string& group, const std::string& key, double value) {
    setDouble(group, key, value);
  }

  void setDefault(const std::string& group, const std::string& key, double value);

  /**
    * Get an integer value from the configuration
    * @param group
    * @param key
    * @return
    */
  int getInt(const std::string& group, const std::string& key) const;
  bool get(const std::string& group, const std::string& key, int& result) const {
    if(hasKey(group,key)) { 
      result = getInt(group, key);
      return true; 
    }
    return false;
  }
  bool get(const std::string& group, const std::string& key, unsigned int& result) const {
    if(hasKey(group,key)) { 
      result = static_cast<unsigned int>(getInt(group, key));
      return true; 
    }
    return false;
  }

  void setInt(const std::string& group, const std::string& key, int value);
  void set(const std::string& group, const std::string& key, int value) {
    setInt(group, key, value);
  }
  void set(const std::string& group, const std::string& key, unsigned int value) {
    setInt(group, key, static_cast<int>(value));
  }

  void setDefault(const std::string& group, const std::string& key, int value);
  void setDefault(const std::string& group, const std::string& key, unsigned int value);

  /**
    * Get a bool value from the configuration
    * @param group
    * @param key
    * @return
    */
  bool getBool(const std::string& group, const std::string& key) const;
  bool get(const std::string& group, const std::string& key, bool& result) const {
    if(hasKey(group,key)) { 
      result = getBool(group, key);
      return true; 
    }
    return false;
  }

  void setBool(const std::string& group, const std::string& key, bool value);
  void set(const std::string& group, const std::string& key, bool value) {
    setBool(group, key, value);
  }
  void setDefault(const std::string& group, const std::string& key, bool value);

  /**
    * Get a raw and uninterpretated string from the configuration
    * @param group
    * @param key
    * @return
    */
  std::string getRawValue(const std::string& group, const std::string& key) const;

  void setRawValue(const std::string& group, const std::string& key, const std::string& value);

private:
  GKeyFile* publicKeyFile;
  GKeyFile* privateKeyFile;
  std::string privateDir;

  // returns true if the path exists and is a directory, false otherwise
  bool isDir(std::string& path) const {
    return g_file_test(path.c_str(), G_FILE_TEST_EXISTS) && g_file_test(path.c_str(), G_FILE_TEST_IS_DIR);
  }

  void loadFromSingleDir(GKeyFile* keyFile, std::string dirlocation, bool required = true);

  void loadFile(GKeyFile* keyFile, std::string file, std::string groupName);

  void saveFile(GKeyFile* keyFile, const std::string& file, const std::string& group);

  GKeyFile* chooseKeyFile(const std::string& group, const std::string& key) const { return ( g_key_file_has_key(privateKeyFile, group.c_str(), key.c_str(), NULL) > 0 ) ? privateKeyFile : publicKeyFile; }
};//end class Configuration

}

#endif  /* CONFIGURATION_H */

