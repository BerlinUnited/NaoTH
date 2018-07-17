#ifndef _TEST_FUNCTIONS
#define _TEST_FUNCTIONS

#include <Eigen/Eigen>
#include <string>
#include <sstream>
#include "math.h"

// implementing some test functions from "Damping Parameter In Marquardt's Mehtod", Hans Bruun Nielsen
namespace nielsen {
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
}

// implementing some test functions from https://en.wikipedia.org/wiki/Test_functions_for_optimization
namespace wikipedia {

    template <int N>
    class Rosenbrock {
    public:
        Rosenbrock(){}

        Eigen::Matrix<double, N, 1> operator ()(const Eigen::Matrix<double, N, 1>& parameter) const {
            Eigen::Matrix<double, N, 1> f;
            for( int i = 0; i < N-2; i++){
                f(i,0) = 100*std::pow(parameter(i+1,0)-parameter(i,0)*parameter(i,0), 2) + std::pow(1 - parameter(i,0), 2);
            }
            f(N-2,0) = 100*std::pow(parameter(N-1,0)-parameter(N-2,0)*parameter(N-1,0), 2);
            f(N-1,0) = std::pow(1 - parameter(N-1,0), 2);

            return f;
        }

        size_t getNumberOfResudials() const{
            return N;
        }

        std::string getName(){
            std::stringstream ss;
            ss << "Rosenbrock " << N << std::endl;
            return ss.str();
        }
    };

    typedef Rosenbrock<2> Rosenbrock2;
    typedef Rosenbrock<3> Rosenbrock3;
    typedef Rosenbrock<7> Rosenbrock7;

    class Himmelblau {
    public:
        Himmelblau(){}

        Eigen::Vector2d operator()(const Eigen::Vector2d &p) const;

        size_t getNumberOfResudials() const{
            return 2;
        }

        std::string getName(){
            return "Himmelblau";
        }

        /* known minima
         * f (  3.0     ,  2.0     ) = 0.0
         * f ( −2.805118,  3.131312) = 0.0
         * f ( −3.779310, −3.283186) = 0.0
         * f (  3.584428, −1.848126) = 0.0
         */
    };

    template <int N>
    class Rastrigin {
    public:
        Rastrigin(){}

        Eigen::Matrix<double, N, 1> operator ()(const Eigen::Matrix<double, N, 1>& p) const {
            Eigen::Matrix<double, N, 1> f;
            for( int i = 0; i < N-1; i++){
                f(i,0) = p(i,0) * p(i,0) - 10 * std::cos(2 * M_PI * p(i,0));
            }
            f(N-1,0) = 10 * N + p(N-1,0) * p(N-1,0) - 10 * std::cos(2 * M_PI * p(N-1,0));

            return f;
        }

        size_t getNumberOfResudials() const{
            return N;
        }

        std::string getName(){
            std::stringstream ss;
            ss << "Rastrigin " << N << std::endl;
            return ss.str();
        }
    };

    typedef Rastrigin<2> Rastrigin2;
    typedef Rastrigin<3> Rastrigin3;
    typedef Rastrigin<7> Rastrigin7;
}

#endif
