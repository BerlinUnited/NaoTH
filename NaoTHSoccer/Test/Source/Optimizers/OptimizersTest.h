// experimental
#if defined(__GNUC__)
#pragma GCC system_header
#endif

#include <iostream>

unsigned long long max_counter = (unsigned long long)(2*1e3);

#include <chrono>
typedef std::chrono::high_resolution_clock::time_point time_point;
typedef std::chrono::high_resolution_clock::duration duration;

// minimizers to test
#include "Tools/Math/Optimizer.h"

// testing functions
#include "TestFunctions.h"

template<class F, template<typename, typename> class O, class T>
class Test{
public:
    O<F,T> optimizer;

    void operator()(const T& start, const T& eps){
        F function;
        T offset;
        T x = start;

        time_point begin;
        time_point end;

        duration sum = duration::zero();

        unsigned long long counter = 0;
        bool valid;
        do {
            begin = std::chrono::high_resolution_clock::now();

            valid = optimizer.minimizeOneStep(function, x, eps, offset);

            end = std::chrono::high_resolution_clock::now();

            sum += end - begin;

            if(valid){
                x += offset;
            }

            counter++;
        } while(optimizer.error > 1e-5 && !optimizer.step_failed() && counter < max_counter);

        std::cout << optimizer.getName() << ": " << function.getName()  << std::endl;
        std::cout << "final error    : " << optimizer.error << std::endl;
        std::cout << "final solution : " << x << std::endl;
        std::cout << "number of steps: " << counter << std::endl;
        std::cout << "average time: " << sum.count()/counter << " ns" << std::endl;
        std::cout << "total time  : " << sum.count() << " ns" << std::endl;
        std::cout << "step failed : " << (optimizer.step_failed() ? "true" : "false") << std::endl;
        std::cout << "---------------------" << std::endl;
    }
};

template<class F, template<typename, typename> class O, class T>
void testDifferentialBased(const T& start, const T& eps) {
    F function;
    O<F,T> optimizer;
    T offset;
    T x = start;

    time_point begin;
    time_point end;

    duration sum = duration::zero();

    unsigned long long counter = 0;
    bool valid;
    do {
        begin = std::chrono::high_resolution_clock::now();

        valid = optimizer.minimizeOneStep(function, x, eps, offset);

        end = std::chrono::high_resolution_clock::now();

        sum += end - begin;

        if(valid){
            x += offset;
        }

        counter++;
    } while(optimizer.error > 1e-5 && !optimizer.step_failed() && counter < max_counter);

    std::cout << optimizer.getName() << ": " << function.getName()  << std::endl;
    std::cout << "final error    : " << optimizer.error << std::endl;
    std::cout << "final solution : " << x << std::endl;
    std::cout << "number of steps: " << counter << std::endl;
    std::cout << "average time: " << sum.count()/counter << " ns" << std::endl;
    std::cout << "total time  : " << sum.count() << " ns" << std::endl;
    std::cout << "step failed : " << (optimizer.step_failed() ? "true" : "false") << std::endl;
    std::cout << "---------------------" << std::endl;
}
