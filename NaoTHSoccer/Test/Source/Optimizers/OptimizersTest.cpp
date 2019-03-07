#include <OptimizersTest.h>
//#include <fstream>
//#include <iostream>

int main(int /*argc*/, char** /*argv*/){
    /*{
        std::ifstream in;
        in.open("out.dat", std::ios::binary | std::ios::in);

        int cols;
        int rows;

        in.read((char*) &cols, sizeof (int));
        in.read((char*) &rows, sizeof (int));

        std::cout << cols << " " << rows << std::endl;

        Eigen::MatrixXd A(rows, cols);
        in.read((char*)A.data(), cols * rows * sizeof(double));

        in.read((char*) &cols, sizeof (int));
        in.read((char*) &rows, sizeof (int));

        Eigen::MatrixXd B(rows, cols);
        in.read((char*)B.data(), cols * rows * sizeof(double));

        in.close();

        Eigen::VectorXd a =  A.colPivHouseholderQr().solve(B);

        std::cout << a << std::endl;
    }*/
/*
    {
        Eigen::Matrix<double, 2, 1> start;
        start << -1, 1;

        Eigen::Vector2d eps;
        eps << 0.001, 0.001;

        Test<wikipedia::Rosenbrock2, Optimizer::GaussNewtonMinimizer        , Eigen::Matrix<double, 2, 1> >* t  = new Test<wikipedia::Rosenbrock2, Optimizer::GaussNewtonMinimizer        , Eigen::Matrix<double, 2, 1> >();
        Test<wikipedia::Rosenbrock2, Optimizer::LevenbergMarquardtMinimizer , Eigen::Matrix<double, 2, 1> >* t1 = new Test<wikipedia::Rosenbrock2, Optimizer::LevenbergMarquardtMinimizer , Eigen::Matrix<double, 2, 1> >();
        Test<wikipedia::Rosenbrock2, Optimizer::LevenbergMarquardtMinimizer2, Eigen::Matrix<double, 2, 1> >* t2 = new Test<wikipedia::Rosenbrock2, Optimizer::LevenbergMarquardtMinimizer2, Eigen::Matrix<double, 2, 1> >();

        (*t)(start, eps);
        (*t1)(start, eps);
        (*t2)(start, eps);

        delete t;
        delete t1;
        delete t2;
    }

    {
        Eigen::Matrix<double, 3, 1> start;
        start << 2,-2,2;

        Eigen::Matrix<double, 3, 1> eps;
        eps << 0.001, 0.001, 0.001;

        testDifferentialBased<wikipedia::Rosenbrock3, Optimizer::GaussNewtonMinimizer        >(start, eps);
        testDifferentialBased<wikipedia::Rosenbrock3, Optimizer::LevenbergMarquardtMinimizer >(start, eps);
        testDifferentialBased<wikipedia::Rosenbrock3, Optimizer::LevenbergMarquardtMinimizer2>(start, eps);
    }

    {
        Eigen::Matrix<double, 7, 1> start;
        start << 2,-2,2,-2,2,-2,2;

        Eigen::Matrix<double, 7, 1> eps;
        eps << 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001;

        testDifferentialBased<wikipedia::Rosenbrock7, Optimizer::GaussNewtonMinimizer        >(start, eps);
        testDifferentialBased<wikipedia::Rosenbrock7, Optimizer::LevenbergMarquardtMinimizer >(start, eps);
        testDifferentialBased<wikipedia::Rosenbrock7, Optimizer::LevenbergMarquardtMinimizer2>(start, eps);
    }

    {
        Eigen::Matrix<double, 2, 1> start;
        start << 0, 0;

        Eigen::Vector2d eps;
        eps << 0.001, 0.001;

        testDifferentialBased<wikipedia::Himmelblau, Optimizer::GaussNewtonMinimizer        >(start, eps);
        testDifferentialBased<wikipedia::Himmelblau, Optimizer::LevenbergMarquardtMinimizer >(start, eps);
        testDifferentialBased<wikipedia::Himmelblau, Optimizer::LevenbergMarquardtMinimizer2>(start, eps);
    }

    {
        Eigen::Matrix<double, 2, 1> start;
        start << -1, 1;

        Eigen::Vector2d eps;
        eps << 0.001, 0.001;

        testDifferentialBased<wikipedia::Rastrigin2, Optimizer::GaussNewtonMinimizer        >(start, eps);
        testDifferentialBased<wikipedia::Rastrigin2, Optimizer::LevenbergMarquardtMinimizer >(start, eps);
        testDifferentialBased<wikipedia::Rastrigin2, Optimizer::LevenbergMarquardtMinimizer2>(start, eps);
    }

    {
        Eigen::Matrix<double, 3, 1> start;
        start << 2,-2,2;

        Eigen::Matrix<double, 3, 1> eps;
        eps << 0.001, 0.001, 0.001;

        testDifferentialBased<wikipedia::Rastrigin3, Optimizer::GaussNewtonMinimizer        >(start, eps);
        testDifferentialBased<wikipedia::Rastrigin3, Optimizer::LevenbergMarquardtMinimizer >(start, eps);
        testDifferentialBased<wikipedia::Rastrigin3, Optimizer::LevenbergMarquardtMinimizer2>(start, eps);
    }

    {
        Eigen::Matrix<double, 7, 1> start;
        start << 2,-2,2,-2,2,-2,2;

        Eigen::Matrix<double, 7, 1> eps;
        eps << 0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.001;

        testDifferentialBased<wikipedia::Rastrigin7, Optimizer::GaussNewtonMinimizer        >(start, eps);
        testDifferentialBased<wikipedia::Rastrigin7, Optimizer::LevenbergMarquardtMinimizer >(start, eps);
        testDifferentialBased<wikipedia::Rastrigin7, Optimizer::LevenbergMarquardtMinimizer2>(start, eps);
    }
*/
    {
        Eigen::Matrix<double, 3, 1> start;
        start << 2,10, 0.7853981633974483;

        Eigen::Matrix<double, 3, 1> eps;
        eps << 0.001, 0.001, 0.001;

        testDifferentialBased<fitting::Linear, Optimizer::GaussNewtonMinimizer>(start, eps);
        testDifferentialBased<fitting::Linear, Optimizer::LevenbergMarquardtMinimizer>(start, eps);
        testDifferentialBased<fitting::Linear, Optimizer::LevenbergMarquardtMinimizer2>(start, eps);

        Optimizer::GaussNewtonMinimizer<fitting::Linear, Eigen::Matrix<double,3,1> > o(1.0, 1.25, 0.0, 100.0, false);
        testDifferentialBased(&o, start, eps);
        Optimizer::LevenbergMarquardtMinimizer<fitting::Linear, Eigen::Matrix<double,3,1> > o2(1.0, 1.25, 0.0, 100.0);
        testDifferentialBased(&o2, start, eps);
        Optimizer::LevenbergMarquardtMinimizer2<fitting::Linear, Eigen::Matrix<double,3,1> > o3(1.0, 1.25, 0.0, 100.0, 2, 2, 3);
        testDifferentialBased(&o3, start, eps);
    }
}
