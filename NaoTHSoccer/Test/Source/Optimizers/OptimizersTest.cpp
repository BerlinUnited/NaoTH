#include <OptimizersTest.h>

int main(int /*argc*/, char** /*argv*/){
    {
        Eigen::Matrix<double, 2, 1> start;
        start << -1, 1;

        Eigen::Vector2d eps;
        eps << 0.001, 0.001;

        testDifferentialBased<wikipedia::Rosenbrock2, Optimizer::GaussNewtonMinimizer,         Eigen::Matrix<double, 2, 1> >(start, eps);
        testDifferentialBased<wikipedia::Rosenbrock2, Optimizer::LevenbergMarquardtMinimizer,  Eigen::Matrix<double, 2, 1> >(start, eps);
        testDifferentialBased<wikipedia::Rosenbrock2, Optimizer::LevenbergMarquardtMinimizer2, Eigen::Matrix<double, 2, 1> >(start, eps);
    }

    {
        Eigen::Matrix<double, 3, 1> start;
        start << 2,-2,2;

        Eigen::Matrix<double, 3, 1> eps;
        eps << 0.001, 0.001, 0.001;

        testDifferentialBased<wikipedia::Rosenbrock3, Optimizer::GaussNewtonMinimizer,         Eigen::Matrix<double, 3, 1> >(start, eps);
        testDifferentialBased<wikipedia::Rosenbrock3, Optimizer::LevenbergMarquardtMinimizer,  Eigen::Matrix<double, 3, 1> >(start, eps);
        testDifferentialBased<wikipedia::Rosenbrock3, Optimizer::LevenbergMarquardtMinimizer2, Eigen::Matrix<double, 3, 1> >(start, eps);
    }

    {
        Eigen::Matrix<double, 7, 1> start;
        start << 2,-2,2,-2,2,-2,2;

        Eigen::Matrix<double, 7, 1> eps;
        eps << 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001;

        testDifferentialBased<wikipedia::Rosenbrock7, Optimizer::GaussNewtonMinimizer,         Eigen::Matrix<double, 7, 1> >(start, eps);
        testDifferentialBased<wikipedia::Rosenbrock7, Optimizer::LevenbergMarquardtMinimizer,  Eigen::Matrix<double, 7, 1> >(start, eps);
        testDifferentialBased<wikipedia::Rosenbrock7, Optimizer::LevenbergMarquardtMinimizer2, Eigen::Matrix<double, 7, 1> >(start, eps);
    }

    {
        Eigen::Matrix<double, 2, 1> start;
        start << 0, 0;

        Eigen::Vector2d eps;
        eps << 0.001, 0.001;

        testDifferentialBased<wikipedia::Himmelblau, Optimizer::GaussNewtonMinimizer,         Eigen::Vector2d >(start, eps);
        testDifferentialBased<wikipedia::Himmelblau, Optimizer::LevenbergMarquardtMinimizer,  Eigen::Vector2d >(start, eps);
        testDifferentialBased<wikipedia::Himmelblau, Optimizer::LevenbergMarquardtMinimizer2, Eigen::Vector2d >(start, eps);
    }

    {
        Eigen::Matrix<double, 2, 1> start;
        start << -1, 1;

        Eigen::Vector2d eps;
        eps << 0.001, 0.001;

        testDifferentialBased<wikipedia::Rastrigin2, Optimizer::GaussNewtonMinimizer,         Eigen::Matrix<double, 2, 1> >(start, eps);
        testDifferentialBased<wikipedia::Rastrigin2, Optimizer::LevenbergMarquardtMinimizer,  Eigen::Matrix<double, 2, 1> >(start, eps);
        testDifferentialBased<wikipedia::Rastrigin2, Optimizer::LevenbergMarquardtMinimizer2, Eigen::Matrix<double, 2, 1> >(start, eps);
    }

    {
        Eigen::Matrix<double, 3, 1> start;
        start << 2,-2,2;

        Eigen::Matrix<double, 3, 1> eps;
        eps << 0.001, 0.001, 0.001;

        testDifferentialBased<wikipedia::Rastrigin3, Optimizer::GaussNewtonMinimizer,         Eigen::Matrix<double, 3, 1> >(start, eps);
        testDifferentialBased<wikipedia::Rastrigin3, Optimizer::LevenbergMarquardtMinimizer,  Eigen::Matrix<double, 3, 1> >(start, eps);
        testDifferentialBased<wikipedia::Rastrigin3, Optimizer::LevenbergMarquardtMinimizer2, Eigen::Matrix<double, 3, 1> >(start, eps);
    }

    {
        Eigen::Matrix<double, 7, 1> start;
        start << 2,-2,2,-2,2,-2,2;

        Eigen::Matrix<double, 7, 1> eps;
        eps << 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001;

        testDifferentialBased<wikipedia::Rastrigin7, Optimizer::GaussNewtonMinimizer,         Eigen::Matrix<double, 7, 1> >(start, eps);
        testDifferentialBased<wikipedia::Rastrigin7, Optimizer::LevenbergMarquardtMinimizer,  Eigen::Matrix<double, 7, 1> >(start, eps);
        testDifferentialBased<wikipedia::Rastrigin7, Optimizer::LevenbergMarquardtMinimizer2, Eigen::Matrix<double, 7, 1> >(start, eps);
    }
}
