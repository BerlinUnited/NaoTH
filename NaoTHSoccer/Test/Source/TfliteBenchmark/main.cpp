#include <chrono>
#include <iostream>
#include <vector>

#include "tfliteexecuter.h"


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
    const float benchmark_time = 5;
    const int warmup_rounds = 10;


    std::vector<BenchmarkParts> benchmarks {
        {"Patch Classifier mbc_aug Batch 1", "Config/mbc_aug.tflite", {1, 16, 16, 1}, 1},
        {"Patch Classifier mbc_aug Batch 2", "Config/mbc_aug.tflite", {2, 16, 16, 1}, 1},
        {"Patch Classifier mbc_aug Batch 4", "Config/mbc_aug.tflite", {4, 16, 16, 1}, 1},
        {"Patch Classifier mbc_aug Batch 8", "Config/mbc_aug.tflite", {8, 16, 16, 1}, 1},
        {"Patch Classifier mbc_aug_float16 Batch 1", "Config/mbc_aug_float16.tflite", {1, 16, 16, 1}, 1},

    };

    for(auto& b : benchmarks) {
        TFLiteExecuter exec;
        exec.loadModelFromFile(b.model_file, b.input_dim, b.num_cpu);

        for(int i = 0; i < warmup_rounds; i++) {
            exec.execute();
        }

        int counter = 0;
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        duration<float> time_span;

        do {
            exec.execute();
            counter++;
            t2 = high_resolution_clock::now();
            time_span = duration_cast<duration<float>>(t2 - t1);
        } while(time_span.count() < benchmark_time);

        printf("%s; %.2f ;cycles/s\n", b.name.c_str(), counter / time_span.count());
        fflush(stdout);
    }
    return 0;
}