#include <string>

#include <lshbox.h>
#include <unistd.h>
#include "createTopK.h"

using std::string;
using namespace lshbox;

typedef float DATATYPE;

enum LSHTYPE {
    KDBQ, /// Zhu, H. (2014) K-means based double-bit quantization for hashing.
    ITQ, /// Gong et al (2013).  Iterative quantization: A procrustean approach to learning binary codes
    DBQ, /// Gong et al (2012) Double-Bit Quantization for Hashing.
    PSD, // Datar, Indyk (2004) Locality-sensitive hashing scheme based on p-stable distributions
    RBS, /// Indyk Motwani (1999) Approximate Nearest Neighbor - Towards Removing the Curse of Dimensionality
    RHP, /// Charikar (2002) Similarity estimation techniques from rounding algorithms
    SH, /// Weiss (2008), Spectral Hashing.
    TH /// Wang et al (2007). Image Similarity Search with Compact Data Structures
};

class LSHWrapper {
    lshbox::itqLsh<DATATYPE> itq;
    lshbox::kdbqLsh<DATATYPE> kdbq;
    lshbox::dbqLsh<DATATYPE> dbq;
    lshbox::psdLsh<DATATYPE> psd;
//    lshbox::rbsLsh<DATATYPE> rbs;
    lshbox::shLsh<DATATYPE> sh;
    LSHTYPE lshtype = ITQ;
public:
    unsigned int M; ///(521) Hash table size
    unsigned int L; /// Number of hash tables
    unsigned int D; /// dimension
    unsigned int S; /// Size of vectors in train
    unsigned int I; /// Training iterations
    unsigned int N = 4; /// Binary code bytes, (has to be less than D)
    unsigned int T = 2; /// Index mode, you can choose 1(CAUCHY) or 2(GAUSSIAN)
    float W = 0.5; /// Window size

    LSHWrapper(LSHTYPE lshtype, int D=2, int M=500, int L=5, int S = 100, int I = 50) :
            M(M), L(L), D(D), S(S), I(I) {
        this->lshtype = lshtype;
        assert (lshtype != RBS); /// not implemented
        assert (lshtype != TH); /// not implemented
        assert (lshtype != RHP); /// not implemented
        std::cout << "LSHWrapper\tD=" << D << "\tM=" << M << "\tL=" << L << "\tS=" << S << "\tI=" << I << std::endl;
    }

    void load(const string& filename){
        switch(lshtype){
            case ITQ: itq.load(filename);break;
            case KDBQ:kdbq.load(filename);break;
            case DBQ: dbq.load(filename);break;
            case PSD: psd.load(filename);break;
//            case RBS: rbs.load(filename);break;
            case SH: sh.load(filename);break;
            default: break;
        }
    }

    void save(const string& filename){
        switch(lshtype){
            case ITQ: itq.save(filename); break;
            case KDBQ: kdbq.save(filename); break;
            case DBQ: dbq.save(filename);break;
            case PSD: psd.save(filename);break;
//            case RBS: rbs.save(filename);break;
            case SH: sh.save(filename);break;
            default: break;
        }
    }

    void query(Benchmark& bench,
               Matrix<float>& data,
               Scanner<Matrix<float>::Accessor>& scanner,
               unsigned int index){
        switch(lshtype){
            case ITQ: itq.query(data[bench.getQuery(index)], scanner);break;
            case KDBQ: kdbq.query(data[bench.getQuery(index)], scanner);break;
            case DBQ: dbq.query(data[bench.getQuery(index)], scanner);break;
            case PSD: psd.query(data[bench.getQuery(index)], scanner);break;
//            case RBS: rbs.query(data[bench.getQuery(index)], scanner);break;
            case SH: sh.query(data[bench.getQuery(index)], scanner);break;
            default: break;
        }
    }

    void setParams(lshbox::Matrix<DATATYPE>& data){
        switch(lshtype) {
            case ITQ:
                assert (N > 0);
                itqLsh<DATATYPE>::Parameter itqp;
                itqp.M = M; ///(521) Hash table size
                itqp.L = L; /// Number of hash tables
                itqp.D = D;
                itqp.N = N; /// Binary code bytes, (has to be less than D)
                assert (itqp.N <= itqp.D);
                itqp.S = S; /// Size of vectors in train
                itqp.I = I; /// Training iterations

                itq.reset(itqp);
                itq.train(data);
                itq.hash(data);
                break;

            case KDBQ:
                kdbqLsh<DATATYPE>::Parameter kdbqp;
                kdbqp.M = M; ///(521) Hash table size
                kdbqp.L = L; /// Number of hash tables
                kdbqp.D = D;
                kdbqp.N = N; /// /// Number of projection dimensions,corresponding to 2*N binary code bytes for each vector
                kdbqp.I = I; /// Training iterations
                kdbq.reset(kdbqp);
                kdbq.train(data);
//                kdbq.hash(data);// no hashing
                break;
            case DBQ:
                dbqLsh<DATATYPE>::Parameter dbqp;
                dbqp.M = M; ///(521) Hash table size
                dbqp.L = L; /// Number of hash tables
                dbqp.D = D;
                dbqp.N = N; /// /// Number of projection dimensions,corresponding to 2*N binary code bytes for each vector
                dbqp.I = I; /// Training iterations
                dbq.reset(dbqp);
                dbq.train(data);
                dbq.train(data);
                break;
            case PSD:
                psdLsh<DATATYPE>::Parameter psdp;
                psdp.M = M; ///(521) Hash table size
                psdp.L = L; /// Number of hash tables
                psdp.D = D;
                psdp.T = T; /// Index mode, you can choose 1(CAUCHY) or 2(GAUSSIAN)
                psdp.W = W; /// Window Size
                psd.reset(psdp);
//                psd.train() /// no training
                psd.hash(data);
                break;
            case SH:
                shLsh<DATATYPE>::Parameter shp;
                shp.M = M; ///(521) Hash table size
                shp.L = L; /// Number of hash tables
                shp.D = D;
                shp.N = N; /// Binary code bytes
                shp.S = S; /// Size of vectors in train
                sh.reset(shp);
                sh.train(data);
                sh.hash(data);
                break;
            default: break;
        }
    }
};


int main(int argc, char *argv[])
{
    bool use_index = false;
    LSHTYPE lshtype = ITQ;
    string data_file;
    string lsh_file;
    string benchmark_file;
    int k = -1;
    string query_file;

    int c; /// character for getopt
    int M=500; ///(521) Hash table size
    int L=5; /// Number of hash tables
    int S = 100; /// Size of vectors in train
    int I = 50; /// Training iterations
    int N = -1; /// Binary code bytes

    while((c =  getopt(argc, argv, "i:x:b:q:t:K:M:L:S:I:N:hu")) != EOF){
        switch (c){
            case 'i': data_file = optarg; break;
            case 'x': lsh_file= optarg; break;
            case 'b': benchmark_file= optarg; break;
            case 'K': k = atoi(optarg); break;
            case 't': lshtype = static_cast<LSHTYPE>(atoi(optarg)); break;
            case 'M': M = atoi(optarg); break;
            case 'L': L = atoi(optarg); break;
            case 'S': S = atoi(optarg); break;
            case 'I': I = atoi(optarg); break;
            case 'N': N = atoi(optarg); break;
            case 'u' : use_index = (bool) atoi(optarg); break;
            case ':': /// No operand
                fprintf(stderr,"Option -%c requires an operand\n", optopt);
                return -1;
            default: break;
        }
    }

    if (argc < 5)
    {
        std::cerr << "Usage: ./itqlsh_test <input file> <index outfile> <benchmark file> <k> <lshtype> [output query file]" << std::endl;
        return -1;
    }

    std::cout << "Running lshtype=" << lshtype << " for " << argv[1] << "\toutput=" << argv[2] <<
        "\tbenchmark=" << argv[3] << " \tK=" << k <<std::endl;
    std::cout << "Using Iterative Quantization" << std::endl << std::endl;
    std::cout << "#LOADING DATA ..." << std::endl;
    lshbox::timer timer;
    lshbox::Matrix<DATATYPE> data(data_file);
    std::cout << "dataloadtime= " << timer.elapsed() << " (s)\tsize=" << data.getSize() << "\tdim=" << data.getDim() <<std::endl;
    timer.restart();
    std::string file(lsh_file);

    LSHWrapper wrapper(
            lshtype,
            data.getDim(),
            M,
            L,
            S,
            I
    );
    if (N != -1){
        wrapper.N = (unsigned int) N;
    }
    if (use_index && std::ifstream(file)) {
        std::cout << "#LOADING INDEX ..." << std::endl;
        wrapper.load(file);
        std::cout << "indexloadtime= " << timer.elapsed() << " (s)" << std::endl;
    } else {
        std::cout << "#CONSTRUCTING INDEX ..." << std::endl;
        wrapper.setParams(data);
        wrapper.save(file);
        std::cout << "buildtime= " << timer.elapsed() << " (s)" << std::endl;
    }

    std::cout << "#LOADING BENCHMARK ..." << std::endl;
    timer.restart();
    lshbox::Matrix<DATATYPE>::Accessor accessor(data);
    lshbox::Metric<DATATYPE> metric((unsigned)data.getDim(), L1_DIST);
    lshbox::Benchmark bench;
    std::string benchmark(benchmark_file);
    bench.load(benchmark);
    unsigned K = bench.getK();

    std::cout << "#RUNNING QUERY ..." << std::endl;
    timer.restart();
    lshbox::Stat cost, recall, precision;
    lshbox::Stat scalcs;

    lshbox::Scanner<lshbox::Matrix<DATATYPE>::Accessor> scanner(
            accessor,
            metric,
            K
    );

    lshbox::progress_display pd(bench.getQ());
    for (unsigned i = 0; i != bench.getQ(); ++i)
    {
        wrapper.query(bench, data, scanner, i);
        scalcs << scanner.cnt();
        recall << bench.getAnswer(i).recall(scanner.topk());
        precision << bench.getAnswer(i).precision(scanner.topk());
        cost << float(scanner.cnt()) / float(data.getSize());
        ++pd;
    }

    std::cout << "meanquerytime= " << timer.elapsed() / bench.getQ() << " (s)" << std::endl;
    std::cout << "RECALL   = " << recall.getAvg() << " +/- " << recall.getStd() << std::endl;
    std::cout << "PRECISION= " << precision.getAvg() << " +/- " << precision.getStd() << std::endl;
    std::cout << "COST     = " << cost.getAvg() << " +/- " << cost.getStd() << std::endl;
    std::cout << "totaldcalcs=" << scalcs.getSum() << "\t avg="<<
            scalcs.getAvg() << " +/- " << scalcs.getStd() << std::endl;


     scanner.topk().genTopk();
     std::vector<std::pair<float, unsigned> > res = scanner.topk().getTopk();
     for (std::vector<std::pair<float, unsigned> >::iterator it = res.begin(); it != res.end(); ++it)
     {
         std::cout << it->second << ": " << it->first << std::endl;
     }
     std::cout << "DISTANCE COMPARISON TIMES= " << scanner.cnt() << std::endl;
}