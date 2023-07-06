#include "TeamMessageStatisticsModule.h"

#include "Representations/Modeling/TeamMessageData.h"
#include <Tools/StringTools.h>
#include "PlatformInterface/Platform.h"

TeamMessageStatisticsModule::TeamMessageStatisticsModule()
{
    getDebugParameterList().add(&params);
}

TeamMessageStatisticsModule::~TeamMessageStatisticsModule()
{
    // TODO: seems it doesn't work as intended!
    // 'disable' this module in the representation
    for(auto& it : getTeamMessageStatistics().data) {
        it.second.lastStatisticsUpdate.setTime(0);
        it.second.lastStatisticsUpdate.setFrameNumber(0);
    }
    getDebugParameterList().remove(&params);
}

void TeamMessageStatisticsModule::execute() {
    //Check, from which robots we have received a message, and update the corresponding statistics
    for (const auto& it: getTeamState().players) {
        const auto& playerNumber = it.first;
        TeamMessageStatistics::Player& player = getTeamMessageStatistics().getPlayer(playerNumber);
        
        // skip my own messages
        if(playerNumber == getPlayerInfo().playerNumber) {
            player.indicator_messageReceived_upToNow = 0.0;
            player.lastStatisticsUpdate = it.second.messageFrameInfo;
            continue;
        }

        double currentMessageInterval = getFrameInfo().getTimeSince(player.lastStatisticsUpdate);

        // Update only with newer information
        if(it.second.messageFrameInfo > player.lastStatisticsUpdate)
        {
            auto& buffer = playerBuffer[playerNumber];

            if(buffer.size() == 0) {
                // "ignore" the first message interval, most likely it's too high and distorts the statistics
                currentMessageInterval = params.initialMessageInterval;
            } else if(player.expectation_xSquared > 0 && currentMessageInterval > player.avgMsgInterval + player.expectation_xSquared * 3) {
                // haven't heard something in a very long time, first interval is too high and distorts the statistics
                // use the 99,73% deviation interval
                currentMessageInterval = player.expectation_xSquared * 3;
            }

            // add the message time interval to the buffer
            buffer.add(currentMessageInterval);

            // calculation via stddev buffer
            player.avgMsgInterval = buffer.getAdjustedAverage();
            player.expectation_xSquared = buffer.getStdDev(player.avgMsgInterval);
            player.varianceMsgInterval = buffer.getVariance(player.avgMsgInterval);

            // set last update time
            player.lastStatisticsUpdate = it.second.messageFrameInfo;
            player.lastMsgInterval = 0;
            player.numOfMessages++;
            player.indicator_messageReceived_upToNow = 0.0;
        } else {
            // didn't receive a new message from teammate, just update the failure probability of the player/robot
            if (player.varianceMsgInterval > 0) {
                player.indicator_messageReceived_upToNow += probability(player.lastMsgInterval, currentMessageInterval, player.avgMsgInterval, player.varianceMsgInterval);
            } else {
                // if there's no variance and the last message is older than the average, it's likely that the player doesn't send any message
                if(currentMessageInterval > player.avgMsgInterval) {
                    player.indicator_messageReceived_upToNow = 1.0;
                }
            }
            player.lastMsgInterval = currentMessageInterval;
        }
    }

    for(const auto& it : getTeamMessageStatistics().data) {
        PLOT(std::string("MessageStatistics:Robot(")+StringTools::toStr(it.first)+std::string("):messageInterval"), it.second.lastMsgInterval);
        PLOT(std::string("MessageStatistics:Robot(")+StringTools::toStr(it.first)+std::string("):averageMessageInterval"), it.second.avgMsgInterval);
        PLOT(std::string("MessageStatistics:Robot(")+StringTools::toStr(it.first)+std::string("):stddevMesssageInterval"), std::sqrt(it.second.varianceMsgInterval));
        PLOT(std::string("MessageStatistics:Robot(")+StringTools::toStr(it.first)+std::string("):indicator"), it.second.indicator_messageReceived_upToNow);
    }
}

double TeamMessageStatisticsModule::probability(double lower, double upper, double avg, double var) {
    if (upper <= 0.0) {
        return 0.0;
    }
    // NOTE: riemann integral with just one "rectangles" can be insuffiecent!
    return riemann_integral(&TeamMessageStatisticsModule::normalDensity, lower, upper, 1, avg, var);
}

double TeamMessageStatisticsModule::riemann_integral(double (TeamMessageStatisticsModule::*func)(double, double, double), double a, double b, int amountOfRectangles, double avg, double var)
{
    double *arguments = new double[amountOfRectangles];
    for (int i = 1; i <= amountOfRectangles; i++) {
        arguments[i-1] = a + i * (b - a)/amountOfRectangles;
    }
    double sum = 0.0;
    double x = a;
    for (int index = 0; index < amountOfRectangles; index++) {
		double newX = arguments[index];
        double value = (this->*func)((x+newX)/2.0, avg, var);
		double area = value * (newX-x);
		sum += area;
		x = newX;
	}
	delete[] arguments;
	return sum;
}
