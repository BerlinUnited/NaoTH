#ifndef TEAMMESSAGETIMESTATISTICS_H
#define TEAMMESSAGETIMESTATISTICS_H

#include <map>

#include "Tools/NaoTime.h"
#include "Tools/DataStructures/Printable.h"
#include "Tools/DataStructures/RingBufferWithSum.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/TeamMessageData.h"

using namespace naoth;

class TeamMessageTimeStatistics : public naoth::Printable
{
public:
    /** Stores message time inforamtions. */
    struct Player
    {
        Player(unsigned int n = 0) : number(n) {}

        /** the playernumber */
        unsigned int number;
        /** last update of this player */
        FrameInfo lastUpdate;
        /** the fastest round trip time / latency and the resulting time offset (difference) */
        long long rtt = 0;
        long long latency = 0;
        long long offset = 0;
        /** Returns the (estimated) timestamp of the player */
        long long getTimeInMilliSeconds() const {
            return naoth::NaoTime::getSystemTimeInMilliSeconds() - offset;
        }
    };

    /** Collection for storing the various player time measure infos */
    std::map<unsigned int, Player> data;
    /** Aggregates the overall rtt of the team. */
    RingBufferWithSum<long long, 100> globalRTT;
    /** Aggregates the overall latency of the team. */
    RingBufferWithSum<long long, 100> globalLatency;

    /**
     * @brief Prints the available time measure infos to the given stream.
     * @param stream the stream, where the infos should be printed to
     */
    virtual void print(std::ostream& stream) const
    {
        stream << "TimeSyncPlayers ("<<data.size()<<"):\n";
        if(data.empty()) {
            stream << "\t[NONE]\n";
        } else {
            for (auto it = data.cbegin(); it != data.cend(); ++it) {
                Player player = it->second;
                stream << "player: " << player.number << ",\n"
                       << "  - rtt: "     << player.rtt << "ms,\n"
                       << "  - latency: " << player.latency << "ms,\n"
                       << "  - offset: "  << player.offset << "ms,\n"
                       << "  - timestamp: " << player.getTimeInMilliSeconds() << "ms"
                       << "\n";
            }
        }
    }

    /**
     * @brief Returns the team message statistic representation of a player with the player number "number".
     *        If there's no player with the number, a new one is created.
     * @param number the player number
     * @return Player reference to the players time measure info
     */
    Player& getPlayer(const unsigned int& number) {
        Player& ply = data[number];
        // new player struct
        if(ply.number == 0) {
            ply.number = number;
        }
        return ply;
    }

    /**
     * @brief Returns the (estimated) timestamp of the player with the given number.
     *        If no information for the number is available, zero is returned.
     * @param number
     * @return timestamp of the player
     */
    long long getPlayersTimeInMilliSeconds(const unsigned int& number) const {
        const auto& it = data.find(number);
        if(it != data.cend()) {
            return it->second.getTimeInMilliSeconds();
        }
        return 0L;
    }
};


#endif // TEAMMESSAGETIMESTATISTICS_H
