#include <EigenPerformance.h>
#include <iostream>
#include <chrono>

using Eigen::Matrix3f;
using Eigen::Matrix;

typedef std::chrono::high_resolution_clock::time_point time_point;
typedef std::chrono::high_resolution_clock::duration duration;

unsigned long long max_counter = (unsigned long long)1e6;

// the compiler is not allowed to optimize out global variables
Matrix3f inverse3x3;
Matrix<float,6,6> inverse6x6;
Matrix<float,4,100> y;
Matrix<float,2,1> y2;

void test_invert(){
    time_point begin;
    time_point end;

    duration sum = duration::zero();
    unsigned long long counter = 0;

    while (counter < max_counter){
        Matrix3f test;
        test << Matrix3f::Random();

        if(test.determinant()){
            begin = std::chrono::high_resolution_clock::now();
            inverse3x3 = test.inverse();
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
        Matrix<float,6,6> test;

        test << Matrix<float,6,6>::Random();

        if(test.determinant()){
            begin = std::chrono::high_resolution_clock::now();
            inverse6x6 = test.inverse();
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
}

void test_sse(){
    time_point begin;
    time_point end;

    duration sum = duration::zero();
    unsigned long long counter = 0;

    Matrix<float, 4,4> A;
    Matrix<float, 4,100> x;

    while (counter < max_counter ){
        A << Matrix<float,4,4>::Random();
        x << Matrix<float,4,100>::Random();

        begin = std::chrono::high_resolution_clock::now();
        y = A*x;
        end = std::chrono::high_resolution_clock::now();

        // we assume, that difference doesn't exeed the size of int
        sum += end - begin;

        counter++;

        if(counter%(max_counter/10) == 0){
            std::cout << "." << std::flush;
        }
    }

    std::cout << std::endl;
    std::cout << "average time 4x4*4x100 float: " << sum.count()/max_counter << " ns" << std::endl;

    sum = duration::zero();
    counter = 0;

    Matrix<float, 2,2> A2;
    Matrix<float, 2,1> x2;

    while (counter < max_counter ){
        A2 << Matrix<float,2,2>::Random();
        x2 << Matrix<float,2,1>::Random();

        begin = std::chrono::high_resolution_clock::now();
        y2 = A2*x2;
        end = std::chrono::high_resolution_clock::now();

        // we assume, that difference doesn't exeed the size of int
        sum += end - begin;

        counter++;

        if(counter%(max_counter/10) == 0){
            std::cout << "." << std::flush;
        }
    }

    std::cout << std::endl;
    std::cout << "average time 2x2*2x1 float: " << sum.count()/max_counter << " ns" << std::endl;
}

int main(int /*argc*/, char** /*argv*/){
    test_invert();
    test_sse();

    return 0;
}
