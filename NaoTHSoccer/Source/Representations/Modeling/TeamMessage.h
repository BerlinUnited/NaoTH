/* 
 * File:   TeamMessage.h
 * Author: thomas
 *
 * Created on 7. April 2009, 09:45
 */

#ifndef _TEAMMESSAGE_H
#define _TEAMMESSAGE_H

#include <map>

#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/Serializer.h"

#include "Representations/Modeling/TeamMessageData.h"


class TeamMessage : public naoth::Printable
{
public:
    // container holding the last incomeing message of the teammates
    std::map<unsigned int, TeamMessageData> data;

    unsigned int dropNoSplMessage = 0;
    unsigned int dropNotOurTeam = 0;
    unsigned int dropNotParseable = 0; // NOTE: currently not used; if key fails it is not parseable!
    unsigned int dropKeyFail = 0;
    unsigned int dropMonotonic = 0;

    TeamMessage() {}

    virtual ~TeamMessage() {}

    virtual void print(std::ostream& stream) const
    {
        for(auto const &it : data) {
            const TeamMessageData& d = it.second;
            d.print(stream);
            stream << "------------------------" << std::endl;
        }
        stream << "active team-members: " << data.size() << "\n"
               << "========================\n"
               << "dropNoSplMessage = " << dropNoSplMessage << "\n"
               << "dropNotOurTeam = "   << dropNotOurTeam   << "\n"
               << "dropNotParseable = " << dropNotParseable << "\n"
               << "dropKeyFail = "      << dropKeyFail      << "\n"
               << "dropMonotonic = "    << dropMonotonic    << "\n"
               << std::endl;

    }
};

namespace naoth {
template<>
class Serializer<TeamMessage>
{
public:
    static void serialize(const TeamMessage& representation, std::ostream& stream);
    static void deserialize(std::istream& stream, TeamMessage& representation);
};
}

#endif  /* _TEAMMESSAGE_H */

