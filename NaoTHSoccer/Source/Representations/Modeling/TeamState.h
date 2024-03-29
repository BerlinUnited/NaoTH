#ifndef TEAMSTATE_H
#define TEAMSTATE_H

#include <map>
#include <iomanip>

#include <Tools/NaoTime.h>
#include "Tools/Math/Pose2D.h"
#include "Tools/DataStructures/Printable.h"
#include "Representations/Modeling/PlayerInfo.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/Infrastructure/Roles.h"
#include "Representations/Modeling/TeamMessageData.h"
#include "Representations/Modeling/TeamMessageNTP.h"

using namespace naoth;

class TeamState : public naoth::Printable
{
public:
    /** Represents data with the timestamp of the update in ms. */
    template<typename T> struct TimestampedData
    {
        public:
            TimestampedData() {}
            TimestampedData(T val) : data(val) {}
            const T& operator()() const { return data; }
            TimestampedData& operator=(const T& data) {
                this->data = data;
                //TODO: does this need to be a 64bit system time or can it be 32bit Nao-Time getNaoTimeInMilliSeconds()?
                //      inside the modules we should always use the Nao-Time so that timestamps stay comparable.
                this->lastUpdate = naoth::NaoTime::getSystemTimeInMilliSeconds();
                return *this;
            }
            const NaoTimestamp& time() const { return lastUpdate; }

        private:
            T data;
            NaoTimestamp lastUpdate = 0; // 0 means never set/updated
    };

    /** Stores timestamped (message) data of a player. */
    struct Player
    {
        /** the playernumber */
        const PlayerNumber number;
        /** the frameinfo when the last message was parsed/received */
        FrameInfo messageFrameInfo;
        /** timestamp in ms when the last message was parsed/received */
        NaoTimestamp messageParsed = 0;
        /** timestamp in ms when the message was send; 0 if it wasn't send */
        NaoTimestamp messageTimestamp = 0;

        /** ntp requests from teammates */
        TimestampedData<std::vector<TeamMessageNTP::Request>> ntpRequests;

        /** state of the robot (initial, ready, set, play, finish, penalized) */
        TimestampedData<PlayerInfo::RobotState> state = PlayerInfo::unstiff;
        /** true means that the robot is fallen, false means that the robot can play */
        TimestampedData<bool> fallen = false;
        /** indicates, whether the robot is ready to walk */
        TimestampedData<bool> readyToWalk = false;
        /** the robot pose */
        TimestampedData<Pose2D> pose;

        // TODO: why ballAge is double, but timeToBall unsigned int?
        /** ms since this robot last saw the ball. -1 if we haven't seen it */
        TimestampedData<double> ballAge = -1.0;
        /** position of ball relative to the robot coordinates in millimeters; 0,0 is in centre of the robot */
        TimestampedData<Vector2d> ballPosition;
        /** the shorest time, in which the robot can reach the ball [ms] */
        TimestampedData<unsigned int> timeToBall = 0;

        /** announcement that the player wants to be striker */
        TimestampedData<bool> wantsToBeStriker = false;
        /** the player decided to be striker */
        TimestampedData<bool> wasStriker = false;
        /** role of the robot (static & dynamic) */
        TimestampedData<Roles::Role> robotRole;

        /* Methods **************************************************************/
        /************************************************************************/

        Player(PlayerNumber n = 0) : number(n) {}

        /**
         * @brief print
         * @param stream
         */
        void print(std::ostream &stream) const
        {
            stream << "------------------------" << std::endl;
            stream << "player: " << number << "\n";
            stream << " - time\n"
                    << "\t" << "send: "          << messageTimestamp << "\n"
                    << "\t" << "received: "      << messageFrameInfo.getFrameNumber() << " @ " << messageFrameInfo.getTime() << "\n"
                    << "\t" << "parsed: "        << messageParsed << "\n";
            stream << "- robot\n"
                    << "\t" << "state: "         << PlayerInfo::toString(state()) << " @ " << state.time() << "\n" // 
                    << "\t" << "fallen: "        << (fallen() ? "yes" : "no") << " @ " << fallen.time() << "\n"
                    << "\t" << "readyToWalk: "   << (readyToWalk() ? "yes" : "no") << " @ " << readyToWalk.time() << "\n"
                    << "\t" << "pos: "           << pose().translation.x << "; " << pose().translation.y << "; " << pose().rotation << " @ " << pose.time() << "\n";
            stream << " - ball\n"
                    << "\t" << "TimeSinceBallwasSeen: " << ballAge() << " @ " << ballAge.time() << "\n"
                    << "\t" << "pos: "           << ballPosition().x << "; " << ballPosition().y << " @ " << ballPosition.time() << "\n"
                    << "\t" << "TimeToBall: "    << timeToBall() << " @ " << timeToBall.time() << "\n";
            stream << " - team\n"
                    << "\t" << "wantsToBeStriker: " << (wantsToBeStriker() ? "yes" : "no") << " @ " << wantsToBeStriker.time() << "\n"
                    << "\t" << "wasStriker: "    << (wasStriker() ? "yes" : "no") << " @ " << wasStriker.time() << "\n"
                    << "\t" << "role: "          << Roles::getName(robotRole().role) << " / " << Roles::getName(robotRole().dynamic) << " @ " << robotRole.time() << "\n";
            if (!ntpRequests().empty()) {
                stream << "\t"
                        << "ntp request for: \n";
                for (auto const& request : ntpRequests()) {
                    stream << "\t\t" << request.playerNumber << ", "
                            << request.sent << " -> " << request.received
                            << std::endl;
                }
            }
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

template<> class Serializer<TeamState>
{
    public:
        static void serialize(const TeamState& representation, std::ostream& stream);
        static void deserialize(std::istream& stream, TeamState& representation);
};

#endif // TEAMSTATE_H
