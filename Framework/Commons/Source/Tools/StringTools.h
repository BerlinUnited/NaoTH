#ifndef STRINGTOOLS_H
#define STRINGTOOLS_H

#include <vector>
#include <sstream>

namespace naoth
{

namespace StringTools
{
  template<typename T>
  bool strTo(const std::string& parameter, T& value)
  {
    std::stringstream ss(parameter);
    ss >> std::boolalpha >> value;
    return true;
  }

  template<typename T>
  std::string toStr(const T& value)
  {
    std::stringstream ss;
    ss << std::boolalpha << value;
    return ss.str();
  }

  inline std::vector<std::string> split(const std::string& strToSplit, char delimeter)
  {
    std::stringstream ss(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;

    while (std::getline(ss, item, delimeter)) {
      splittedStrings.push_back(item);
    }

    return splittedStrings;
  }
} // namespace StringTools

} // namespace naoth

#endif // STRINGTOOLS_H
