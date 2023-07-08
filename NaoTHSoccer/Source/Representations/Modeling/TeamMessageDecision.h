#ifndef TEAMMESSAGEDECISION_H
#define TEAMMESSAGEDECISION_H

#include "Tools/DataStructures/Printable.h"
#include <Messages/TeamMessage.pb.h>
#include <Tools/DataConversion.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

class TeamMessageDecision : public naoth::Printable
{
public:
    struct BooleanValue
    {
        public:
            void set() { value = true; ++cnt; }
            void reset() { value = false; }
            const bool operator()() const { return value; }
            friend std::ostream& operator<<(std::ostream& stream, const BooleanValue& val) {
                return stream << (val() ? "True" : "False") << " (" << val.cnt << ")";
            }
            unsigned int count() { return cnt; }

           private:
            bool value = false;
            unsigned int cnt = 0;
    };

    /** ntp requests from teammates */
    BooleanValue send_ntpRequests;

    /** state of the robot (initial, ready, set, play, finish, penalized) */
    BooleanValue send_state;
    /** true means that the robot is fallen, false means that the robot can play */
    BooleanValue send_fallen;
    /** indicates, whether the robot is ready to walk */
    BooleanValue send_readyToWalk;
    /** the robot pose */
    BooleanValue send_pose;

    /** ms since this robot last saw the ball. -1 if we haven't seen it */
    BooleanValue send_ballAge;
    /** position of ball relative to the robot coordinates in millimeters; 0,0 is in centre of the robot */
    BooleanValue send_ballPosition;
    /** the shorest time, in which the robot can reach the ball [ms] */
    BooleanValue send_timeToBall;

    /** announcement that the player wants to be striker */
    BooleanValue send_wantsToBeStriker;
    /** the player decided to be striker */
    BooleanValue send_wasStriker;
    /** role of the robot (static & dynamic) */
    BooleanValue send_robotRole;

    /** Resets all decisions. */
    void reset()
    {
        send_ntpRequests.reset();
        send_state.reset();
        send_fallen.reset();
        send_readyToWalk.reset();
        send_pose.reset();
        send_ballAge.reset();
        send_ballPosition.reset();
        send_timeToBall.reset();
        send_wantsToBeStriker.reset();
        send_wasStriker.reset();
        send_robotRole.reset();
    }

    /** Print the decisions. */
    virtual void print(std::ostream& stream) const
    {
        stream  << "ntpRequests = " << send_ntpRequests << "\n"
                << "state = " << send_state << "\n"
                << "fallen = " << send_fallen << "\n"
                << "readyToWalk = " << send_readyToWalk << "\n"
                << "pose = " << send_pose << "\n"
                << "ballAge = " << send_ballAge << "\n"
                << "ballPosition = " << send_ballPosition << "\n"
                << "timeToBall = " << send_timeToBall << "\n"
                << "wantsToBeStriker = " << send_wantsToBeStriker << "\n"
                << "wasStriker = " << send_wasStriker << "\n"
                << "robotRole = " << send_robotRole << "\n"
        ;
    }
};

template<> class Serializer<TeamMessageDecision>
{
    public:
        static void serialize(const TeamMessageDecision& representation, std::ostream& stream)
        {
            naothmessages::TeamMessageDecision decision;
            decision.set_send_ntprequests(representation.send_ntpRequests());
            decision.set_send_state(representation.send_state());
            decision.set_send_fallen(representation.send_fallen());
            decision.set_send_readytowalk(representation.send_readyToWalk());
            decision.set_send_pose(representation.send_pose());
            decision.set_send_ballage(representation.send_ballAge());
            decision.set_send_ballposition(representation.send_ballPosition());
            decision.set_send_timetoball(representation.send_timeToBall());
            decision.set_send_wantstobestriker(representation.send_wantsToBeStriker());
            decision.set_send_wasstriker(representation.send_wasStriker());
            decision.set_send_robotrole(representation.send_robotRole());

            google::protobuf::io::OstreamOutputStream buf(&stream);
            decision.SerializeToZeroCopyStream(&buf);
        }

        static void deserialize(std::istream& stream, TeamMessageDecision& representation)
        {
            naothmessages::TeamMessageDecision decision;
            google::protobuf::io::IstreamInputStream buf(&stream);
            decision.ParseFromZeroCopyStream(&buf);

            representation.reset();
            if (decision.send_ntprequests()) { representation.send_ntpRequests.set(); }
            if (decision.send_state()) { representation.send_state.set(); }
            if (decision.send_fallen()) { representation.send_fallen.set(); }
            if (decision.send_readytowalk()) { representation.send_readyToWalk.set(); }
            if (decision.send_pose()) { representation.send_pose.set(); }
            if (decision.send_ballage()) { representation.send_ballAge.set(); }
            if (decision.send_ballposition()) { representation.send_ballPosition.set(); }
            if (decision.send_timetoball()) { representation.send_timeToBall.set(); }
            if (decision.send_wantstobestriker()) { representation.send_wantsToBeStriker.set(); }
            if (decision.send_wasstriker()) { representation.send_wasStriker.set(); }
            if (decision.send_robotrole()) { representation.send_robotRole.set(); }
        }
};

#endif // TEAMMESSAGEDECISION_H
