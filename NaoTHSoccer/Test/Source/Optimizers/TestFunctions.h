#include <Eigen/Eigen>

#ifndef _TEST_FUNCTIONS
#define _TEST_FUNCTIONS

class Rosenbrock {
public:
    Rosenbrock(){}

    Eigen::Vector2d operator()(const Eigen::Matrix<double, 2, 1>& parameter) const;

    size_t getNumberOfResudials() const{
        return 2;
    }
};

#endif
