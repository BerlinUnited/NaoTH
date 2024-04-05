#include "KickType.h"

KickType::KickType(std::vector<double> t_X, std::vector<double> f_X,
                   std::vector<double> t_Z, std::vector<double> f_Z) {
    t_X_Points = t_X;
    f_X_Points = f_X;
    t_Z_Points = t_Z;
    f_Z_Points = f_Z;
}
KickType::KickType() { ASSERT_MSG(false, "Should not happen"); }