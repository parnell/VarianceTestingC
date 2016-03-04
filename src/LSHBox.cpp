#include <string>

#include <lshbox.h>
using std::string;

int main(int argc, char const *argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage: ./itqlsh_test <input file> <index outfile> <benchmark file> <k>" << std::endl;
        return -1;
    }
    string data_file(argv[1]);
    string lsh_file(argv[2]);
    string benchmark_file(argv[3]);
    int k = atoi(argv[4]);

    std::cout << "Using Iterative Quantization" << std::endl << std::endl;
    typedef float DATATYPE;
    std::cout << "LOADING DATA ..." << std::endl;
    lshbox::timer timer;
    lshbox::Matrix<DATATYPE> data(data_file);
    std::cout << "LOAD TIME: " << timer.elapsed() << "s." << std::endl;
    std::cout << "CONSTRUCTING INDEX ..." << std::endl;
    timer.restart();
    std::string file(lsh_file);
    bool use_index = false;
    lshbox::itqLsh<DATATYPE> mylsh;
    if (use_index)
    {
        mylsh.load(file);
    }
    else
    {
        lshbox::itqLsh<DATATYPE>::Parameter param;
        param.M = 521;
        param.L = 5;
        param.D = data.getDim();
        param.N = 8;
        param.S = 100;
        param.I = 50;
        mylsh.reset(param);
        mylsh.train(data);
    }
    mylsh.save(file);
    std::cout << "CONSTRUCTING TIME: " << timer.elapsed() << "s." << std::endl;
    std::cout << "LOADING BENCHMARK ..." << std::endl;
    timer.restart();
    lshbox::Matrix<DATATYPE>::Accessor accessor(data);
    lshbox::Metric<DATATYPE> metric(data.getDim(), L1_DIST);
    lshbox::Benchmark bench;
    std::string benchmark(benchmark_file);
    bench.load(benchmark);
    unsigned K = bench.getK();
    lshbox::Scanner<lshbox::Matrix<DATATYPE>::Accessor> scanner(
            accessor,
            metric,
            K
    );
    std::cout << "LOADING TIME: " << timer.elapsed() << "s." << std::endl;
    std::cout << "RUNING QUERY ..." << std::endl;
    timer.restart();
    lshbox::Stat cost, recall, precision;
    lshbox::progress_display pd(bench.getQ());
    for (unsigned i = 0; i != bench.getQ(); ++i)
    {
        scanner.reset(data[bench.getQuery(i)]);
        mylsh.query(data[bench.getQuery(i)], scanner);
        scanner.topk().genTopk();
        recall << bench.getAnswer(i).recall(scanner.topk());
        recall << bench.getAnswer(i).precision(scanner.topk());
        cost << float(scanner.cnt()) / float(data.getSize());
        ++pd;
    }
    std::cout << "MEAN QUERY TIME: " << timer.elapsed() / bench.getQ() << "s." << std::endl;
    std::cout << "RECALL   : " << recall.getAvg() << " +/- " << recall.getStd() << std::endl;
    std::cout << "PRECISION: " << recall.getAvg() << " +/- " << recall.getStd() << std::endl;
    std::cout << "COST     : " << cost.getAvg() << " +/- " << cost.getStd() << std::endl;
    std::cout << "Distance Calcs     : " << mylsh.scalcs.getSum() << "   avg="<<
            mylsh.scalcs.getAvg() << " +/- " << mylsh.scalcs.getStd() << std::endl;

    // scanner.reset(data[0]);
    // mylsh.query(data[0], scanner);
    // scanner.topk().genTopk();
    // std::vector<std::pair<float, unsigned> > res = scanner.topk().getTopk();
    // for (std::vector<std::pair<float, unsigned> >::iterator it = res.begin(); it != res.end(); ++it)
    // {
    //     std::cout << it->second << ": " << it->first << std::endl;
    // }
    // std::cout << "DISTANCE COMPARISON TIMES: " << scanner.cnt() << std::endl;
}