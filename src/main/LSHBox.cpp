#include <string>

#include <lshbox.h>
using std::string;

int main(int argc, char const *argv[])
{
    if (argc != 5)
    {
        std::cerr << "Usage: ./itqlsh_test <input file> <index outfile> <benchmark file> <k>" << std::endl;
        return -1;
    }
    string data_file(argv[1]);
    string lsh_file(argv[2]);
    string benchmark_file(argv[3]);
    int k = atoi(argv[4]);
    std::cout << "Running lsh for " << argv[1] << "  output=" << argv[2] <<
    "  benchmark=" << argv[3] << " K=" << k <<std::endl;
    std::cout << "Using Iterative Quantization" << std::endl << std::endl;
    typedef float DATATYPE;
    std::cout << "LOADING DATA ..." << std::endl;
    lshbox::timer timer;
    lshbox::Matrix<DATATYPE> data(data_file);
    std::cout << "loadtime= " << timer.elapsed() << " (s) << size=" << data.getSize() << ", dim=" << data.getDim() <<std::endl;
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
        param.M = 500; ///(521) Hash table size
        param.L = 5; /// Number of hash tables
        param.D = (unsigned) data.getDim();
        param.N = 2; /// Binary code bytes, (has to be less than D)
        assert (param.N <= param.D);
        param.S = 100; /// Size of vectors in train
        param.I = 50; /// Training iterations
        mylsh.reset(param);
        mylsh.train(data);
    }
    mylsh.save(file);
    std::cout << "buildtime= " << timer.elapsed() << "s." << std::endl;
    std::cout << "LOADING BENCHMARK ..." << std::endl;
    timer.restart();
    lshbox::Matrix<DATATYPE>::Accessor accessor(data);
    lshbox::Metric<DATATYPE> metric((unsigned)data.getDim(), L1_DIST);
    lshbox::Benchmark bench;
    std::string benchmark(benchmark_file);
    bench.load(benchmark);
    unsigned K = bench.getK();
    lshbox::Scanner<lshbox::Matrix<DATATYPE>::Accessor> scanner(
            accessor,
            metric,
            K
    );
    std::cout << "loadbenchmarktime= " << timer.elapsed() << "s." << std::endl;
    std::cout << "RUNNING QUERY ..." << std::endl;
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
    std::cout << "meanquerytime= " << timer.elapsed() / bench.getQ() << "s." << std::endl;
    std::cout << "RECALL   = " << recall.getAvg() << " +/- " << recall.getStd() << std::endl;
    std::cout << "PRECISION= " << recall.getAvg() << " +/- " << recall.getStd() << std::endl;
    std::cout << "COST     = " << cost.getAvg() << " +/- " << cost.getStd() << std::endl;
    std::cout << "totaldcalcs=" << mylsh.scalcs.getSum() << "   , avg="<<
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