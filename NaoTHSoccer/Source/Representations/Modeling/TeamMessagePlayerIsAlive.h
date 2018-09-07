#ifndef TEAMMESSAGEPLAYERISALIVE_H
#define TEAMMESSAGEPLAYERISALIVE_H

#include <map>

#include "Tools/NaoTime.h"
#include "Tools/DataStructures/Printable.h"

using namespace naoth;

class TeamMessagePlayerIsAlive : public naoth::Printable
{
public:
    /** Collection for storing the 'dead/alive' status of each player */
    std::map<unsigned int, bool> data;

    /**
     * @brief Returns the number of alive robots.
     * @return number of alive robots
     */
    unsigned int getAliveCount() const {
        unsigned int count = 0;
        for(const auto& it : data) {
            count += it.second;
        }
        return count;
    }

    /**
     * @brief Returns the number of 'dead' robots.
     * @return number of 'dead' robots
     */
    unsigned int getDeadCount() const {
        unsigned int count = 0;
        for(const auto& it : data) {
            count += !it.second;
        }
        return count;
    }

    /**
     * @brief Returns the 'ALIVE' status of a given player number.
     * @param player_number the player number to check
     * @return true, if player is 'ALIVE', false otherwise
     */
    bool inline isAlive(unsigned int player_number) const {
        const auto& player = data.find(player_number);
        if(player != data.cend()) {
            return player->second;
        }
        return false;
    }

    /**
     * @brief Returns the 'DEAD' status of a given player number.
     * @param player_number the player number to check
     * @return true, if player is 'DEAD', false otherwise
     */
    bool inline isDead(unsigned int player_number) const {
        const auto& player = data.find(player_number);
        if(player != data.cend()) {
            return !player->second;
        }
        return true;
    }

    /**
     * @brief Returns the 'ALIVE' status of a given player number.
     * @param player_number the player number to check
     * @return true, if player is 'ALIVE', false otherwise
     */
    bool operator [](unsigned int player_number) const {
        return isAlive(player_number);
    }

    /**
     * @brief Prints the available time measure infos to the given stream.
     * @param stream the stream, where the infos should be printed to
     */
    virtual void print(std::ostream& stream) const
    {
        stream << "TeamMessagePlayerIsAlive ("<<data.size()<<"):\n";
        if(data.empty()) {
            stream << "\t[NONE]\n";
        } else {
            // iterate through players and print data
            for (const auto& it : data) {
                stream << "\t Robot #" << it.first << ": " << (it.second?"true":"false") << "\n";
            }
        }
    }
};

#endif // TEAMMESSAGEPLAYERISALIVE_H
