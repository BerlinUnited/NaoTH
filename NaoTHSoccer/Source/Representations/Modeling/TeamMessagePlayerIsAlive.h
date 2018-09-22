#ifndef TEAMMESSAGEPLAYERISALIVE_H
#define TEAMMESSAGEPLAYERISALIVE_H

#include <map>
#include <algorithm>

#include "Tools/NaoTime.h"
#include "Tools/DataStructures/Printable.h"

using namespace naoth;

class TeamMessagePlayerIsAlive : public naoth::Printable
{
public:
    /** Collection for storing the 'dead/alive' status of each player */
    std::map<unsigned int, bool> data;

    /**
     * Collection for storing the 'active' state of each player (is playing on the field).
     * A player can be alive, but not active, eg. when he's penalized.
     */
    std::map<unsigned int, bool> active;

    /**
     * @brief Returns the number of alive robots.
     * @return number of alive robots
     */
    long getAliveCount() const {
        return std::count_if(data.cbegin(), data.cend(), [](const std::pair<unsigned int, bool>& it){ return it.second; });
    }

    /**
     * @brief Returns the number of 'dead' robots.
     * @return number of 'dead' robots
     */
    long getDeadCount() const {
        return std::count_if(data.cbegin(), data.cend(), [](const std::pair<unsigned int, bool>& it){ return !it.second; });
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
     * @brief Returns the number of active robots (playing on the field).
     *        A player can be alive, but not active, eg. when he's penalized.
     * @return number of active robots
     */
    long getActiveCount() const {
        return std::count_if(active.cbegin(), active.cend(), [](const std::pair<unsigned int, bool>& it){ return it.second; });
    }

    /**
     * @brief Returns the 'ACTIVE' status of a given player number.
     *        If the player number wasn't found in the active map, the 'alive' state is
     *        returned instead - the active state wasn't calculated, but the player could
     *        be still alive.
     * @param player_number the player number to check
     * @return true, if player is 'ACTIVE', false otherwise
     */
    bool inline isActive(unsigned int player_number) const {
        const auto& player = active.find(player_number);
        if(player != active.cend()) {
            return player->second;
        }
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
                stream << "\t Robot #" << it.first << ": "
                       << (it.second?"true":"false")
                       << " & " << (isActive(it.first)?"active":"inactive")
                       << "\n";
            }
        }
    }
};

#endif // TEAMMESSAGEPLAYERISALIVE_H
