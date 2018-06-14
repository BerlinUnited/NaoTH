//  g++ -O2 -o test test.cpp

#include <stdint.h>
//#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <cassert>
#include <random>
#include <chrono>

typedef std::chrono::high_resolution_clock::time_point time_point;
typedef std::chrono::high_resolution_clock::duration duration;

using namespace std;

const size_t N = 1000;
long long bigarray[N+2];

std::default_random_engine generator;

template <class T>
void runtest() {
    time_point begint;
    time_point endt;

    int repeat = 20;

    std::uniform_int_distribution<T> distribution(-1000, 1000);

    cout << "size of word: ";
    cout << sizeof(T) << endl;
    for(unsigned int offset = 0; offset<sizeof(T);++offset) {
        cout << "offset = "<< offset << endl;
        T * const begin = reinterpret_cast<T *> (reinterpret_cast<uintptr_t>(bigarray) + offset);
        assert(offset + reinterpret_cast<uintptr_t>(bigarray)  == reinterpret_cast<uintptr_t>(begin));

        duration sumt = duration::zero();
        T val = 0;
        for(int k = 0 ; k < repeat; ++k) {
            for(size_t i = 0; i < N;++i) {
                begin[i] = distribution(generator);
            }

            begint = std::chrono::high_resolution_clock::now();
                for(size_t i = 0; i < N;++i) {
                    val += begin[i]; // don't care for over or underflow
                }
            endt = std::chrono::high_resolution_clock::now();

            sumt += endt - begint;

        }
        cout << val;
        cout << endl;
        cout << " average time for offset "<<(offset%sizeof(T))<<" is "<<sumt.count()/repeat<<endl;
	}

}



int main() {
    cout << "run test for int:" << endl;
    runtest<int>();
    cout << endl;

    cout << "run test for long:" << endl;
	runtest<long>();
    cout << endl;

    cout << "run test for long long:" << endl;
	runtest<long long>();
	cout<<endl;

	return 0;
}
