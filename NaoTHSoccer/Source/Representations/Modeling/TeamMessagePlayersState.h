#ifndef TEAMMESSAGEPLAYERSSTATE_H
#define TEAMMESSAGEPLAYERSSTATE_H

#include <map>
#include <algorithm>

#include "Tools/DataStructures/Printable.h"

using namespace naoth;

class TeamMessagePlayersState : public naoth::Printable
{
public:
    /** Struct containing the states of the teammates, which can be retrieved of the teammessage. */
    struct Player
    {
        /** A player is 'alive', if he's continously sending messages. */
        bool alive = false;
        /** A player is 'active', if he's 'alive' and is in the 'playing' gamestate (or 'ready', 'set'). */
        bool active = false;
        /** A player is 'penalized', if he's 'penalized'. */
        bool penalized = false;
        /** A player is 'playing', if he's not fallen. */
        bool playing = false;

        /**
         * @brief Returns true, if the player is 'alive' & 'active'.
         * @return true|false
         */
        bool isActive() const { return alive && active; }

        /**
         * @brief Returns true, if the player is 'alive' & 'penalized'.
         * @return true|false
         */
        bool isPenalized() const { return alive && penalized; }

        /**
         * @brief Returns true, if the player is 'alive' & 'active' & 'playing'.
         * @return true|false
         */
        bool isPlaying() const { return alive && active && playing; }
    };

    /** Container for the robot states. */
    std::map<unsigned int, Player> data;

    /**
     * @brief Returns the number of alive robots.
     * @return number of alive robots
     */
    long getAliveCount() const {
        return std::count_if(data.cbegin(), data.cend(), [](const std::pair<unsigned int, Player>& it){ return it.second.alive; });
    }

    /**
     * @brief Returns the number of 'dead' robots.
     *        Dead means, the robot didn't send a new teammessage in a certain time intervall.
     * @return number of 'dead' robots
     */
    long getDeadCount() const {
        return std::count_if(data.cbegin(), data.cend(), [](const std::pair<unsigned int, Player>& it){ return !it.second.alive; });
    }

    /**
     * @brief Returns the 'ALIVE' status of a given player number.
     * @param player_number the player number to check
     * @return true, if player is 'ALIVE', false otherwise
     */
    bool inline isAlive(unsigned int player_number) const {
        const auto& player = data.find(player_number);
        if(player != data.cend()) {
            return player->second.alive;
        }
        return false;
    }

    /**
     * @brief Returns the 'DEAD' status of a given player number.
     *        Dead means, the robot didn't send a new teammessage in a certain time intervall.
     * @param player_number the player number to check
     * @return true, if player is 'DEAD', false otherwise
     */
    bool inline isDead(unsigned int player_number) const {
        const auto& player = data.find(player_number);
        if(player != data.cend()) {
            return !player->second.alive;
        }
        return true;
    }

    /**
     * @brief Returns the number of active robots (on the field).
     *        A player can be alive, but not active, eg. when he's penalized.
     * @return number of active robots
     */
    long getActiveCount() const {
        return std::count_if(data.cbegin(), data.cend(), [](const std::pair<unsigned int, Player>& it){ return it.second.isActive(); });
    }

    /**
     * @brief Returns the 'ACTIVE' status of a given player number.
     *        A player is 'ACTIVE' if he's alive and on the field.
     * @param player_number the player number to check
     * @return true, if player is 'ACTIVE', false otherwise
     */
    bool inline isActive(unsigned int player_number) const {
        const auto& player = data.find(player_number);
        if(player != data.cend()) {
            return player->second.isActive();
        }
        return false;
    }

    /**
     * @brief Returns the number of playing robots (playing on the field / not fallen).
     *        'Playing' means, that the teammate can be used for team related decisions.
     * @return number of 'playing' robots
     */
    long getPlayingCount() const {
        return std::count_if(data.cbegin(), data.cend(), [](const std::pair<unsigned int, Player>& it){ return it.second.isPlaying(); });
    }

    /**
     * @brief Returns the 'PLAYING' status of a given player number.
     *        'Playing' means, that the teammate can be used for team related decisions.
     * @param player_number the player number to check
     * @return true, if player is 'PLAYING', false otherwise
     */
    bool inline isPlaying(unsigned int player_number) const {
        const auto& player = data.find(player_number);
        if(player != data.cend()) {
            return player->second.isPlaying();
        }
        return false;
    }

    /**
     * @brief Prints the available time measure infos to the given stream.
     * @param stream the stream, where the infos should be printed to
     */
    virtual void print(std::ostream& stream) const
    {
        stream << "TeamMessagePlayersState ("<<data.size()<<"):\n";
        if(data.empty()) {
            stream << "\t[NONE]\n";
        } else {
            // iterate through players and print data
            for (const auto& it : data) {
                stream << "\t Robot #" << it.first << ": "
                       << (it.second.alive?"alive":"dead")
                       << " & " << (it.second.active?"active":"inactive")
                       << " & " << (it.second.playing?"playing":"NOT playing")
                       << "\n";
            }
        }
    } // END print
};

#endif // TEAMMESSAGEPLAYERSSTATE_H
