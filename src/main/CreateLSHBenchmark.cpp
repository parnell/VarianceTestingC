#include <iostream>
#include <vector>

#include <flann/flann.hpp>
#include "vec2hdf5.h"
#include "stats/fstat.h"
#include <lshbox.h>
#include <unistd.h>




using namespace flann;


int main(int argc, char** argv)
{
    if (argc < 6)
    {
        std::cerr << "Usage: ./CreateLSHBenchmark <input file> <output file> "
                << "<k> <# queries> <fold> -V <output vec query file> -M <output ms query file -r or -k needed > " << std::endl;
        return -1;
    }
    int c;

    std::string file(argv[1]);
    std::string bench_file(argv[2]);
    unsigned K = (unsigned) atoi(argv[3]);
    unsigned Q = (unsigned) atoi(argv[4]);
    unsigned seed = (unsigned) atoi(argv[5]);

    std::string query_file = argv[7];
    std::string msquery_file = argv[9];

    int msK = atoi(argv[11]);
    double msR = 0.0f;

    /// no idea why getopt is always returning -1
    while ((c = getopt(argc, const_cast<char * const * >(argv), "V:M:r:k:")) != EOF){
        switch (c){
            case 'V': query_file = optarg; break;
            case 'M': msquery_file = optarg; break;
            case 'r': msR = atof(optarg); break;
            case 'k': msK = atoi(optarg); break;
            case ':': /// No operand
                fprintf(stderr,"Option -%c requires an operand\n", optopt);
                return -1;
            default: break;
        }
    }

    lshbox::timer timer;
    timer.restart();

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
        fstat dist, tkdist;
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
            dist.addStat(distance);
        }
        topk.genTopk();
        std::vector<std::pair<float, unsigned> >& tops = topk.getTopk();
        for (auto e : tops){
            qdistances.addStat(e.first);
            tkdist.addStat(e.first);
        }
        ++pd;
        std::cout << "\nquery=" << i<< "\t time=" << timer2.elapsed() << " (s).\tquerytopkmeandist=" <<
            tkdist.getMean() <<"\tquerymeandist=" << dist.getMean() << std::endl;
    }
    bench.save(bench_file);
    double total_time = timer.elapsed();
    std::cout << "meantime=" << total_time / Q << " (s)\ttotaltime="<<total_time << "\ttopkmeandist=" << qdistances.getMean() <<
            "\t meandist=" << distances.getMean() << std::endl;

    std::cout << "loadbenchmarktime= " << timer.elapsed() << " (s)" << std::endl;
    const size_t dim = (size_t) data.getDim();
    if (!query_file.empty()){
        std::cout << "#Creating Query File " << query_file <<  std::endl;

        std::ofstream ofs (query_file, std::ofstream::out);
        ofs << data.getDim() << ' ' << bench.getQ() << " 2" << std::endl;
        for (unsigned i = 0; i != bench.getQ(); ++i) {
            unsigned q = bench.getQuery(i);
            for (unsigned j = 0; j != dim; ++j){
                ofs << data[q][j];
                if (j!=dim-1){
                    ofs << ' ';
                }
            }
            ofs << std::endl;
        }
        ofs.close();
    }

    if (!msquery_file.empty()){
        std::cout << "#Creating MS Query File " << msquery_file << std::endl;

        std::ofstream ofs (msquery_file, std::ofstream::out);
        for (unsigned i = 0; i != bench.getQ(); ++i) {
            unsigned q = bench.getQuery(i);
            for (unsigned j = 0; j != dim; ++j){
                if (j==0){
                    if (msK > 0){
                        ofs << -msK << ',';
                    } else {
                        ofs << msR << ',';
                    }
                }
                ofs << data[q][j];
                if (j!=dim-1){
                    ofs << ',';
                }
            }
            ofs << std::endl;
        }
        ofs << "-0" << std::endl;
        ofs.close();
    }


}
