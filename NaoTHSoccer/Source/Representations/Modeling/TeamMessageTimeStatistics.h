#ifndef TEAMMESSAGETIMESTATISTICS_H
#define TEAMMESSAGETIMESTATISTICS_H

#include <map>

#include "Tools/NaoTime.h"
#include "Tools/DataStructures/Printable.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Modeling/TeamMessageData.h"

using namespace naoth;

class TeamMessageTimeStatistics : public naoth::Printable
{
public:
    /** */
    struct Player
    {
        Player(unsigned int n = 0) : number(n) {}

        /** the playernumber */
        unsigned int number;
        /** last update of this player */
        FrameInfo lastUpdate;
        /** */
        long long rtt = 0.0;
        long long latency = 0.0;
        long long offset = 0.0;
        /** */
        long long getTimeInMilliSeconds() {
            return naoth::NaoTime::getSystemTimeInMilliSeconds() - offset;
        }
    };

    /** Collection for storing the various player syncing infos */
    std::map<unsigned int, Player> data;

    /**
     * @brief Prints the available syncing infos to the given stream.
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
                       //<< "  - buffer: " << player.getBufferSize() << ",\n"
                       << "  - rtt: "     << player.rtt << "ms,\n"
                       << "  - latency: " << player.latency << "ms,\n"
                       << "  - offset: "  << player.offset << "ms,\n"
                       //<< "  - avg.offset: " << player.getAverageOffset() << "ms,\n"
                       << "  - timestamp: " << player.getTimeInMilliSeconds() << "ms"
                       << "\n";
            }
        }
    }

    /**
     * @brief Returns the team message statistic representation of a player with the player number "number".
     *        If there's no player with the number, "nullptr" is returned!
     * @param number the player number
     * @return pointer to the player representation or nullptr
     */
    Player& getPlayer(const unsigned int& number) {
        Player& ply = data[number];
        // new player struct
        if(ply.number == 0) {
            ply.number = number;
        }
        return ply;
    }
};


#endif // TEAMMESSAGETIMESTATISTICS_H
