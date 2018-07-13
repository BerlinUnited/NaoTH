#include <OptimizersTest.h>
#include <iostream>
#include <chrono>

typedef std::chrono::high_resolution_clock::time_point time_point;
typedef std::chrono::high_resolution_clock::duration duration;

unsigned long long max_counter = (unsigned long long)1e6;
unsigned long long counter;

// the compiler is not allowed to optimize out global variables

void test_LM_Rosenbrock(){
    Rosenbrock rb;
    Eigen::Vector2d x;
    x << -1.2, 1;

    Eigen::Vector2d eps;
    eps << 0.001, 0.001;

    Eigen::Vector2d offset;

    LevenbergMarquardtMinimizer<Rosenbrock> lm(1);

    time_point begin;
    time_point end;

    duration sum = duration::zero();

    counter = 0;
    bool valid;
    do {
        begin = std::chrono::high_resolution_clock::now();

        valid = lm.minimizeOneStep2(rb, x, eps, offset);

        end = std::chrono::high_resolution_clock::now();

        sum += end - begin;

        if(valid){
            x += offset;
        }

        counter++;

        //std::cout << lm.error << std::endl;
    } while(lm.error > 1e-5 && !lm.NaNed() && counter < max_counter);

    std::cout << std::endl;
    std::cout << "final error    : " << lm.error << std::endl;
    std::cout << "final solution : " << x << std::endl;
    std::cout << "number of steps: " << counter << std::endl;
    std::cout << "average time ln: " << sum.count()/counter << " ns" << std::endl;
    std::cout << "total time ln  : " << sum.count() << " ns" << std::endl;

    lm.reset();
    x << -1.2, 1;
    sum = duration::zero();

    counter = 0;
    double error;
    do {
        begin = std::chrono::high_resolution_clock::now();

        offset = lm.minimizeOneStep(rb, x, eps, error);

        end = std::chrono::high_resolution_clock::now();

        sum += end - begin;

        x += offset;

        counter++;

        //std::cout << lm.error << std::endl;
    } while(error > 1e-5 && !lm.NaNed() && counter < max_counter);

    std::cout << std::endl;
    std::cout << "final error    : " << error << std::endl;
    std::cout << "final solution : " << x << std::endl;
    std::cout << "number of steps: " << counter << std::endl;
    std::cout << "average time ln: " << sum.count()/counter << " ns" << std::endl;
    std::cout << "total time ln  : " << sum.count() << " ns" << std::endl;
}

void test_GN_Rosenbrock(){
    Rosenbrock rb;
    Eigen::Matrix<double, 2, 1> x;
    x << -1.2, 1;

    double eps(0.001);

    Eigen::Matrix<double, 2, 1> offset;

    GaussNewtonMinimizer<2,2,Rosenbrock> gn;

    time_point begin;
    time_point end;

    duration sum = duration::zero();

    counter = 0;
    do {
        begin = std::chrono::high_resolution_clock::now();

        offset = gn.minimizeOneStep(rb, x, eps);

        end = std::chrono::high_resolution_clock::now();

        sum += end - begin;

        x += offset;

        counter++;

        //std::cout << gn.error << std::endl;
    } while(gn.error > 1e-5 && !gn.NaNed() && counter < max_counter);

    std::cout << std::endl;
    std::cout << "final error    : " << gn.error << std::endl;
    std::cout << "final solution : " << x << std::endl;
    std::cout << "number of steps: " << counter << std::endl;
    std::cout << "average time gn: " << sum.count()/counter << " ns" << std::endl;
    std::cout << "total time gn  : " << sum.count() << " ns" << std::endl;
}

int main(int /*argc*/, char** /*argv*/){
    test_LM_Rosenbrock();
    test_GN_Rosenbrock();

    return 0;
}
