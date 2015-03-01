#include "PlainKalmanFilterBallLocator.h"

PlainKalmanFilterBallLocator::PlainKalmanFilterBallLocator()
{

}

PlainKalmanFilterBallLocator::~PlainKalmanFilterBallLocator()
{

}

void PlainKalmanFilterBallLocator::execute()
{
    // prediction
    x_pre = F * x + B * u + w;

    P_pre = F * P * F.transpose() + Q;

    // fusion/update
    K = P_pre * H.transpose() * (H * P_pre * H.transpose() + R).inverse();

    x_corr = x_pre + K * (z - H * x_pre );

    P_corr = P_pre - K*H*P_pre;
}
