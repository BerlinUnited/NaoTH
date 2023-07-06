#ifndef TEAMMESSAGEDECISION_H
#define TEAMMESSAGEDECISION_H

#include "Tools/DataStructures/Printable.h"

class TeamMessageDecision : public naoth::Printable
{
public:
    struct BooleanValue
    {
        public:
            void set() { value = true; }
            void reset() { value = false; }
            const bool operator()() const { return value; }
            friend std::ostream& operator<<(std::ostream& stream, const BooleanValue& val) {
                return stream << (val() ? "True" : "False");
            }

        private:
            bool value = false;
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

#endif // TEAMMESSAGEDECISION_H
