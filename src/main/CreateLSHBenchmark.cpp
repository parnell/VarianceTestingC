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
    timer.restart();

    std::string file(argv[1]);
    std::string query_file(argv[2]);
    K = (unsigned) atoi(argv[3]);
    Q = (unsigned) atoi(argv[4]);
    std::cout << "CREATE BENCHMARK FOR DATA " << argv[1] << "  output=" << argv[2] <<
            "  k=" << K << " Q=" << Q <<std::endl;
    lshbox::Matrix<float> data(file);

    lshbox::Benchmark bench;
    bench.init(Q, K, (unsigned) data.getSize(), seed);

    lshbox::Metric<float> metric((unsigned) data.getDim(), L2_DIST);
    lshbox::progress_display pd(Q);
    fstat qdistances, distances;
    for (unsigned i = 0; i != Q; ++i)
    {
        lshbox::timer timer2;
        timer2.restart();

        unsigned q = bench.getQuery(i);
        lshbox::Topk &topk = bench.getAnswer(i);
        for (unsigned j = 0; j != data.getSize(); ++j)
        {
            if (q == j) {
                continue; }
            float distance = metric.dist(data[q], data[j]);
            topk.push(j, distance);
            distances.addStat(distance);
        }
        topk.genTopk();
        std::vector<std::pair<float, unsigned> >& tops = topk.getTopk();
        for (auto e : tops){
            qdistances.addStat(e.first);
        }
        ++pd;
        std::cout << "\nquery=" << i<< " , time=" << timer2.elapsed() << " (s)." << std::endl;
    }
    bench.save(query_file);
    double total_time = timer.elapsed();
    std::cout << "meantime=" << total_time / Q << " (s), totaltime="<<total_time << ", topkmeandist=" << qdistances.getMean() <<
            " , meandist=" << distances.getMean() <<
            std::endl;
}