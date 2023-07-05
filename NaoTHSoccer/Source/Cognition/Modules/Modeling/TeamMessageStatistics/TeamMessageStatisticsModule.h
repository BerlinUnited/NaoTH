/*
* @file TeamMessageStatistics.h
*
* @author <a href="mailto:schahin.tofangchi@hu-berlin.de">Schahin Tofangchi</a>
* Statistics for the whole team
*/

#ifndef _TeamMessageStatisticsModule_H
#define _TeamMessageStatisticsModule_H

#include <ModuleFramework/Module.h>
#include <Representations/Infrastructure/FrameInfo.h>
#include "Representations/Modeling/PlayerInfo.h"

#include "Tools/DataStructures/ParameterList.h"
#include "Tools/DataStructures/RingBufferWithStddev.h"
#include "Tools/Debug/DebugParameterList.h"
#include "Tools/Debug/DebugPlot.h"
#include "Tools/Debug/DebugRequest.h"

#include "Representations/Modeling/TeamState.h"
#include "Representations/Modeling/TeamMessageStatistics.h"


BEGIN_DECLARE_MODULE(TeamMessageStatisticsModule)
  PROVIDE(DebugPlot)
  PROVIDE(DebugRequest)
  PROVIDE(DebugParameterList)
  
  REQUIRE(FrameInfo)
  REQUIRE(PlayerInfo)
  REQUIRE(TeamState)

  PROVIDE(TeamMessageStatistics)
END_DECLARE_MODULE(TeamMessageStatisticsModule)


class TeamMessageStatisticsModule: public TeamMessageStatisticsModuleBase
{
public:
    TeamMessageStatisticsModule();
    virtual ~TeamMessageStatisticsModule();

    virtual void execute();

    /**
     * @brief Calculates the probability that a message will now be received from the other player.
     * @param lower
     * @param upper
     * @return
     */
    double probability(double lower, double upper, double avg, double var);

    /**
     * @brief Computes the Riemann Integral of a function for the given interval [a, b] and an amount of rectangles.
     * @param a
     * @param b
     * @param amountOfRectangles
     * @return
     */
    double riemann_integral(double (TeamMessageStatisticsModule::*func)(double, double, double), double a, double b, int amountOfRectangles, double avg, double var);

    /**
     * Probability distribution function of a normal distribution
     *
     * @brief f(x|\mu ,\sigma ^{2})={\frac {1}{\sqrt {2\pi \sigma ^{2}}}}e^{-{\frac {(x-\mu )^{2}}{2\sigma ^{2}}}}
     * @param x
     * @param mu        the average of the distribution
     * @param sigma_2   the variance of the distribution
     * @return          the probability at point x
     */
    inline double normalDensity(double x, double mu, double sigma_2) {
        return  1.0
                / std::sqrt(2 * Math::pi * sigma_2)
                * std::exp( -Math::sqr(x - mu) / (2 * sigma_2) );
    }

private:
    class Parameters: public ParameterList
    {
    public:
        Parameters(): ParameterList("TeamMessageStatisticsModule") {
            PARAMETER_REGISTER(interpolation) = 0.0;
            PARAMETER_REGISTER(initialMessageInterval) = 350;
            // load from the file after registering all parameters
            syncWithConfig();
        }
        virtual ~Parameters() {}

        /**
         * @brief Determines the weighting of newly received message intervals (set to 0 for unweighted average)
         */
        double interpolation;

        /**
         * @brief The time interval which should be used if we heard the first time from the teammate.
         *        Should be approximately the same as the message sending interval.
         */
        int initialMessageInterval;
    } params;

    /**
     * @brief Buffer for the players (receiving) message interval.
     */
    std::map<unsigned int, RingBufferWithStddev<double, 100>> playerBuffer;
};

#endif  /* _TeamMessageStatisticsModule_H */
