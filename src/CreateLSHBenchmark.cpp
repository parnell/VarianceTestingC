#include <iostream>
#include <vector>

#include <flann/flann.hpp>
#include <flann/io/hdf5.h>
#include "vec2hdf5.h"
#include "stats/fstat.h"
#include "extras.h"
#include <signal.h>
#include <unistd.h>
#include <execinfo.h>


using namespace std;
using namespace flann;

void handler(int sig) {
    void *array[10];
    int size = backtrace(array, 10);

    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}


#include <lshbox.h>
int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        std::cerr << "Usage: ./CreateLSHBenchmark <input file> <output file> <k> <# queries>" << std::endl;
        return -1;
    }
    unsigned K = 100, Q = 1000, seed = 0;
    lshbox::timer timer;
    std::cout << "CREATE BENCHMARK FOR DATA ..." << std::endl;
    timer.restart();

    std::string file(argv[1]);
    std::string query_file(argv[2]);
    K = atoi(argv[3]);
    Q = atoi(argv[4]);
    lshbox::Matrix<float> data(file);

    lshbox::Benchmark bench;
    bench.init(Q, K, data.getSize(), seed);

    lshbox::Metric<float> metric(data.getDim(), L2_DIST);
    lshbox::progress_display pd(Q);
    for (unsigned i = 0; i != Q; ++i)
    {
        unsigned q = bench.getQuery(i);
        lshbox::Topk &topk = bench.getAnswer(i);
        for (unsigned j = 0; j != data.getSize(); ++j)
        {
            if (q == j)
            {
                continue;
            }
            topk.push(j, metric.dist(data[q], data[j]));
        }
        topk.genTopk();
        ++pd;
    }
    bench.save(query_file);
    std::cout << "MEAN QUERY TIME: " << timer.elapsed() / Q << "s." << std::endl;
}