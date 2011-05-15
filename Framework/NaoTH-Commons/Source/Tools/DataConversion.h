/**
* @file DataConversion.h
* 
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* convert data to string or string to data
*/

#ifndef _DATACONVERSION_H
#define	_DATACONVERSION_H

#include <cstring>
#include <sstream>

namespace DataConversion
{
  template<typename T>
  bool strTo(const std::string& parameter, T& value)
  {
    std::stringstream ss(parameter);
    ss >> std::boolalpha >> value;
    return true;
  }//end strto

  template<typename T>
  std::string toStr(const T& value)
  {
    std::stringstream ss;
    ss << std::boolalpha << value;
    return ss.str();
  }
};

#endif	/* _DATACONVERSION_H */


