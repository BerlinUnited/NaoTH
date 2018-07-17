#include <OptimizersTest.h>

int main(int /*argc*/, char** /*argv*/){
    Eigen::Matrix<double, 2, 1> start;
    start << -1.2, 1;

    Eigen::Vector2d eps;
    eps << 0.001, 0.001;

    testDifferentialBased<Rosenbrock, Optimizer::GaussNewtonMinimizer,         Eigen::Matrix<double, 2, 1> >(start, eps);
    testDifferentialBased<Rosenbrock, Optimizer::LevenbergMarquardtMinimizer,  Eigen::Matrix<double, 2, 1> >(start, eps);
    testDifferentialBased<Rosenbrock, Optimizer::LevenbergMarquardtMinimizer2, Eigen::Matrix<double, 2, 1> >(start, eps);

    return 0;
}
