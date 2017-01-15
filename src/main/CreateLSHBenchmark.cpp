#include <iostream>
#include <vector>

#include <flann/flann.hpp>
#include "vec2hdf5.h"
#include "stats/fstat.h"
#include <lshbox.h>


using namespace std;
using namespace flann;


int main(int argc, char *argv[])
{
    if (argc != 6)
    {
        std::cerr << "Usage: ./CreateLSHBenchmark <input file> <output file> <k> <# queries> <output query file>" << std::endl;
        return -1;
    }
    unsigned K = 100, Q = 1000, seed = 0;
    lshbox::timer timer;
    timer.restart();

    std::string file(argv[1]);
    std::string bench_file(argv[2]);
    K = (unsigned) atoi(argv[3]);
    Q = (unsigned) atoi(argv[4]);
    std::string query_file(argv[5]);
    std::cout << "CREATE BENCHMARK FOR DATA=" << argv[1] << "  output=" << argv[2] <<
            "\tk=" << K << "\tQ=" << Q << "\tqueryfile=" << argv[5] <<std::endl;
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
        std::cout << "\nquery=" << i<< "\t time=" << timer2.elapsed() << " (s)." << std::endl;
    }
    bench.save(bench_file);
    double total_time = timer.elapsed();
    std::cout << "meantime=" << total_time / Q << " (s)\ttotaltime="<<total_time << "\ttopkmeandist=" << qdistances.getMean() <<
            "\t meandist=" << distances.getMean() << std::endl;

    std::cout << "loadbenchmarktime= " << timer.elapsed() << " (s)" << std::endl;

    if (!query_file.empty()){
        std::cout << "#Creating Query File" << std::endl;

        std::ofstream ofs (query_file, std::ofstream::out);
        ofs << data.getDim() << " " << bench.getQ() << " 2" << std::endl;
        for (unsigned i = 0; i != bench.getQ(); ++i) {
            unsigned q = bench.getQuery(i);
            for (unsigned j = 0; j != data.getDim(); ++j){
                ofs << data[q][j];
                if (j!=K-1){
                    ofs << " ";
                }
            }
            ofs << std::endl;
        }
        ofs.close();
    }


}
