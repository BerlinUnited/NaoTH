#include <chrono>
#include <iostream>
#include <vector>

#include "tfliteexecuter.h"
#include <iostream>

using namespace std::chrono;
using namespace std;


struct BenchmarkParts {
    std::string name;
    std::string model_file;
    std::vector<int> input_dim;
    int num_cpu = 1;
};

int main(int argc, char *argv[])
{
    // copied from htwk
    std::vector<BenchmarkParts> benchmarks {
        {"Dummy Model 2", "Config/dummy_model2.tflite", {1, 16, 16, 1}, 1},
    };

    for(auto& b : benchmarks) {
        TFLiteExecuter exec;
        exec.loadModelFromFile(b.model_file, b.input_dim, b.num_cpu);

        exec.execute();

        std::cout << exec.getOutputTensor() << std::endl;
        
    }
    return 0;
}