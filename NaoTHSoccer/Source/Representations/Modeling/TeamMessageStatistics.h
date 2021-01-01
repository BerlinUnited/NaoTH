#ifndef TEAMMESSAGESTATISTICS_H
#define TEAMMESSAGESTATISTICS_H

#include <map>
#include <iomanip>

#include "Tools/DataStructures/Printable.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/TeamMessageData.h"

using namespace naoth;

class TeamMessageStatistics : public naoth::Printable
{
public:
    /** Stores message statistic inforamtions. */
    struct Player
    {
        /** the playernumber */
        unsigned int number;
        /** last statistics update of this player */
        FrameInfo lastStatisticsUpdate;
        /** interval of the last calculation */
        double lastMsgInterval = 0.0;
        /** Amount of (new) messages received */
        unsigned long numOfMessages = 0;
        /** Average interval between a pair of messages sent by the same robot */
        double avgMsgInterval = 0.0;
        /** Sum of squares of the message intervals */
        double expectation_xSquared = 0.0;
        /** Variance of the interval between a pair of messages sent by the same robot */
        double varianceMsgInterval = 0.0;
        /**
         * Indicator that a message should be received right now.
         *   0.5    would be the avg. receiving time.
         *   1.0    sometihng near '1.0' means, that the player is probably 'dead'.
         */
        double indicator_messageReceived_upToNow = 0.0;

        /* Methods **************************************************************/
        /************************************************************************/

        Player(unsigned int n = 0) : number(n) {}

        bool isStatisticsActive() const { return lastStatisticsUpdate.getFrameNumber() > 0; }

        /**
         * @brief print
         * @param stream
         */
        void print(std::ostream &stream) const
        {
            stream << "player: " << number << ",\n";

            if(lastStatisticsUpdate.getFrameNumber() > 0) {

                stream << "  - last statistics update: "  << lastStatisticsUpdate.getFrameNumber()
                                                          << " @ " << lastStatisticsUpdate.getTime() << ",\n"
                       << "  - messages: "                << numOfMessages << ",\n"
                       << "  - last message: "            << lastMsgInterval << "ms,\n"
                       << "  - avg. receiving time: "     << avgMsgInterval << "ms,\n"
                       << "  - sqr. receiving time: "     << expectation_xSquared << "ms,\n"
                       << "  - var. receiving time: "     << varianceMsgInterval << "ms,\n"
                       << "  - indicator receiving msg: " << std::fixed << std::setprecision(6)
                                                          << indicator_messageReceived_upToNow << ",\n";
            } else {
                stream << "  - [[ no statistics data ]]\n";
            }
        }
    };

    /** Collection for storing the various player time measure infos */
    std::map<unsigned int, Player> data;

    /**
     * @brief Prints the available time measure infos to the given stream.
     * @param stream the stream, where the infos should be printed to
     */
    virtual void print(std::ostream& stream) const
    {
        stream << "TeamMessageStatistics ("<<data.size()<<"):\n";
        if(data.empty()) {
            stream << "\t[NONE]\n";
        } else {
            // iterate through players and print data
            for (const auto& it : data) {
                it.second.print(stream);
            }
        }
    }

    /**
     * @brief Returns the team message statistic representation of a player with the player number "number".
     *        If there's no player with the number, a new one is created.
     * @param number the player number
     * @return Player reference to the players time measure info
     */
    Player& getPlayer(unsigned int number) {
        Player& ply = data[number];
        // new player struct
        if(ply.number == 0) {
            ply.number = number;
        }
        return ply;
    }

    /**
     * @brief Returns the team message statistic representation of a player with the player number "number".
     * @param number the player number
     * @return Player reference to the players time measure info
     * @throw  std::out_of_range  If no such player is present.
     */
    const Player& getPlayer(unsigned int number) const {
        return data.at(number);
    }

    /**
     * @brief isStatisticsActive
     * @param player_number
     * @return
     */
    bool isStatisticsActive(unsigned int player_number) const {
        const auto& player = data.find(player_number);
        return player != data.cend() && player->second.isStatisticsActive();
    }

    /**
     * @brief Indicator whether we expect another message for the given player.
     *        0.0 means we're just got a new message, something near 1.0 means that the player
     *        should already send a message -> he's probably dead!
     *        This method is only valid if the 'TeamMessageStatisticsModule' is active.
     * @param player_number
     * @return
     */
    double getMessageIndicator(unsigned int player_number) const {
        auto it = data.find(player_number);
        if (it != data.end() && it->second.isStatisticsActive()) {
            return it->second.indicator_messageReceived_upToNow;
        }
        return 1.0;
    }
};


#endif // TEAMMESSAGESTATISTICS_H
