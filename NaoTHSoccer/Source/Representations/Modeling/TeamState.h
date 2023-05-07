#ifndef TEAMSTATE_H
#define TEAMSTATE_H

#include <map>
#include <iomanip>

#include <Tools/NaoTime.h>
#include "Tools/DataStructures/Printable.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/TeamMessageData.h"

typedef unsigned int PlayerNumber;

using namespace naoth;

class TeamState : public naoth::Printable
{
public:
    /** Represents data with the timestamp of the update in ms. */
    template<typename T> struct TimestampedData
    {
        public:
            const T& operator()() const { return data; }
            TimestampedData& operator=(const T& data) { this->data = data; this->lastUpdate = naoth::NaoTime::getSystemTimeInMilliSeconds(); }
            const unsigned long long& time() const { return lastUpdate; }

        private:
            T data;
            unsigned long long lastUpdate = naoth::NaoTime::getSystemTimeInMilliSeconds();
    };

    /** Stores timestamped (message) data of a player. */
    struct Player
    {
        /** the playernumber */
        const PlayerNumber number;
        /** timestamp in ms when the message was send; 0 if it wasn't send */
        unsigned long long messageTimestamp;

        TimestampedData<std::vector<NtpRequest>> ntpRequests;
        TimestampedData<Pose2D> pose;
        TimestampedData<double> ballAge;
        TimestampedData<Vector2d> ballPosition;

        /* Methods **************************************************************/
        /************************************************************************/

        Player(PlayerNumber n = 0) : number(n) {}

        /**
         * @brief print
         * @param stream
         */
        void print(std::ostream &stream) const
        {
            stream << "player: " << number << ",\n";
            stream << " - " << "Pos (x; y; rotation) = "
                   << pose().translation.x << "; "
                   << pose().translation.y << "; "
                   << pose().rotation << " @ "
                   << pose.time() <<"\n"
            ;
        }
    };

    /** Collection for storing the various player infos */
    std::map<PlayerNumber, Player> players;

    /**
     * @brief Prints the timestamped infos of the players to the given stream.
     * @param stream the stream, where the infos should be printed to
     */
    virtual void print(std::ostream& stream) const
    {
        stream << "TeamState ("<<players.size()<<"):\n";
        if(players.empty()) {
            stream << "\t[NONE]\n";
        } else {
            // iterate through players and print data
            for (const auto& it : players) {
                it.second.print(stream);
            }
        }
    }

    /**
     * @brief Checks, if infos of a player are available.
     * @param number the player number
     * @return true, if player is available, false otherwise
     */
    bool hasPlayer(PlayerNumber number) const
    {
        return players.find(number) != players.end();
    }

    /**
     * @brief Returns the players informations.
     *        If there's no player with the number, a new one is created.
     * @param number the player number
     * @return Player reference to the players infos
     */
    Player& getPlayer(PlayerNumber number) {
        return players.emplace(std::make_pair(number, Player(number))).first->second;
    }

    /**
     * @brief Returns the players informations.
     * @param number the player number
     * @return Player reference to the players time measure info
     * @throw  std::out_of_range  If no such player is present.
     */
    const Player& getPlayer(PlayerNumber number) const {
        return players.at(number);
    }
};


#endif // TEAMSTATE_H
