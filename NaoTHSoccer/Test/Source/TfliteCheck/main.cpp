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
        {"movenet", "Config/movenet_lightning.tflite", {1, 192, 192, 3}, 1},
    };

    for(auto& b : benchmarks) {
        TFLiteExecuter exec;
        exec.loadModelFromFile(b.model_file, b.input_dim, b.num_cpu);

        exec.execute();

        // tensor: float32[1,1,17,3]
        std::cout << exec.getOutputTensor() << std::endl;
        
        const float (*output)[1][1][17][3] = reinterpret_cast<const float(*)[1][1][17][3]>(exec.getOutputTensor());
        
        for(int i = 0; i < 17; i++) {
            for (int j = 0; j < 3; j++) {
              std::cout << (*output)[0][0][i][j] << "\t";
            }
            std::cout << std::endl;
        }
        
    }
    return 0;
}