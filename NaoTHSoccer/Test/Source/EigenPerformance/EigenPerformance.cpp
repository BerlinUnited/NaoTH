#include <EigenPerformance.h>
#include <iostream>
#include <chrono>

using Eigen::Matrix3d;
using Eigen::Matrix;

typedef std::chrono::high_resolution_clock::time_point time_point;
typedef std::chrono::high_resolution_clock::duration duration;

int main(int /*argc*/, char** /*argv*/){
    unsigned long long max_counter = 1e6;

    time_point begin;
    time_point end;

    duration sum = duration::zero();
    unsigned long long counter = 0;

    while (counter < max_counter){
        Matrix3d test;
        Matrix3d inverse;

        test << Matrix3d::Random();

        if(test.determinant()){
            begin = std::chrono::high_resolution_clock::now();
            inverse = test.inverse();
            end = std::chrono::high_resolution_clock::now();

            // we assume, that difference doesn't exeed the size of int
            sum += end - begin;

            counter++;

            if(counter%(max_counter/10) == 0){
               std::cout << "." << std::flush;
            }
        }
    }

    std::cout << std::endl;
    std::cout << "average time 3x3: " << sum.count()/counter << " ns" << std::endl;

    sum = duration::zero();
    counter = 0;

    while (counter < max_counter ){
        Matrix<double,6,6> test;
        Matrix<double,6,6> inverse;

        test << Matrix<double,6,6>::Random();

        if(test.determinant()){
            begin = std::chrono::high_resolution_clock::now();
            inverse = test.inverse();
            end = std::chrono::high_resolution_clock::now();

            // we assume, that difference doesn't exeed the size of int
            sum += end - begin;

            counter++;

            if(counter%(max_counter/10) == 0){
               std::cout << "." << std::flush;
            }
        }
    }

    std::cout << std::endl;
    std::cout << "average time 6x6: " << sum.count()/max_counter << " ns" << std::endl;

    return 0;
}
