#ifndef STRINGTOOLS_H
#define STRINGTOOLS_H

#include <vector>
#include <sstream>

namespace naoth
{

namespace StringTools
{
    inline std::vector<std::string> split(const std::string& strToSplit, char delimeter)
    {
        std::stringstream ss(strToSplit);
        std::string item;
        std::vector<std::string> splittedStrings;
        while (std::getline(ss, item, delimeter))
        {
           splittedStrings.push_back(item);
        }
        return splittedStrings;
    }
} // namespace StringTools

} // namespace naoth

#endif // STRINGTOOLS_H
