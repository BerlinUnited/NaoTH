#ifndef _TEST_FUNCTIONS
#define _TEST_FUNCTIONS

#include <Eigen/Eigen>
#include <string>

class Rosenbrock {
public:
    Rosenbrock(){}

    Eigen::Vector2d operator()(const Eigen::Matrix<double, 2, 1>& parameter) const;

    size_t getNumberOfResudials() const{
        return 2;
    }

    std::string getName(){
        return "Rosenbrock";
    }
};

#endif
