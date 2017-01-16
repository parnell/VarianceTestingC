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

enum IndexType{
    UNKNOWN,
    KDTREE,
    LSH,
    FLANN_LSH,
    LAST_TYPE
};


struct IndexWrapper{
    NNIndex<L2<float>>* pindex;
    IndexType type;
    IndexWrapper() :pindex(0), type(UNKNOWN){ }
    void resetCalcs(){
        switch(type) {
            case KDTREE:
                flann::L2<float>::distance_calcs=0;
                break;
            case LSH:
                flann::L2<float>::distance_calcs=0;
                break;
            default:
                break;
        }
    }
    size_t getCalcs(){
        switch(type){
            case KDTREE:
                return flann::L2<float>::distance_calcs;
            case LSH:
                return flann::L2<float>::distance_calcs;
            default:
                break;
        }
        return 0;
    }
};

size_t calcKNNDistCalculations(
        IndexWrapper* pwrap,
        const Matrix<float>& dataset,
        const Matrix<float>& query,
        const size_t nn){
    NNIndex<L2<float>> *pindex = pwrap->pindex;
    printf("\nquery rows = %ld, query cols=%ld,  nn=%ld , veclen=%ld\n", query.rows, query.cols, nn, pindex->veclen());
    Matrix<int> result_indices(new int[query.rows*nn], query.rows, nn);
    Matrix<float> dists(new float[query.rows*nn], query.rows , nn); /// distances of the knn results

    utime64 start = localClock();
    pindex->buildIndex();
    utime64 end = localClock();

    pwrap->resetCalcs();
    pindex->knnSearch(query, result_indices, dists, nn, SearchParams(-1));
    utime64 qend = localClock();
    size_t dc = pwrap->getCalcs();


    printf("Ending KNN\t buildtime=%f\t querytime=%f\t avgquerytime=%f\t knn=%ld\t queries=%ld\t totalcalcs=%ld\t avg=%f\t cost=%f\n",
           (double)(end - start)/1000000,
           (double)(qend - end)/1000000,
           (double)(qend - end)/1000000/query.rows,
           nn, query.rows, dc, (float)dc / query.rows, ((float)dc/ query.rows / pindex->size())/100 );

    /// cleanup
    delete[] result_indices.ptr();
    delete[] dists.ptr();
    return 0;
}

size_t calcRadiusDistCalculations(IndexWrapper* pwrap,
                               const Matrix<float>& dataset,
                               const Matrix<float>& query,
                               const float radius){
    NNIndex<L2<float>> *pindex = pwrap->pindex;

    vector< vector<size_t> > result_indices;
    vector< vector<float> > dists;


    utime64 start = localClock();
    pindex->buildIndex();
    utime64 end = localClock();

    pwrap->resetCalcs();
    pindex->radiusSearch(query, result_indices, dists, radius, SearchParams(-1));
    utime64 qend = localClock();

    size_t nresults =0;
    for (size_t i = 0; i < result_indices.size(); ++i) {
        nresults += result_indices[i].size();
    }
    size_t dc = pwrap->getCalcs();
    printf("buildtime=%f, querytime=%f, avgquerytime=%f, radius%f, queries=%ld, totalcalcs=%ld, avg=%f, size=%zu, avgresults=%f\n",
           (double)(end - start)/1000000,
           (double)(qend - end)/1000000,
           (double)(qend - end)/1000000/query.rows,
           radius,
           query.rows,
           dc ,
           (float) dc/ query.rows,
           nresults ,
           (float) nresults/query.rows);
    return 0;
}


string typeToString(IndexType type){
    switch(type){
        case KDTREE: return "kd-tree";
        case LSH: return "lsh";
        default: return "undefined";
    }
}

int run(int argc, char const * const argv[]) {
    cout << "Starting argc=" << argc << endl;
    for (int k = 0; k < argc; ++k) {
        cout << argv[k] << " ";
    }
    cout << endl;
    string filename;
    string queryFilename;

    float radius = 0.4;
    string indexName = "data";
    int c;
    int knn = -1;
    IndexType index_type = KDTREE;


    int dims=-1;
    int nclusters = -1;
    int size = -1;
    while((c =  getopt(argc, const_cast<char * const * >(argv), "k:y:c:s:d:r:i:q:n:")) != EOF){
        switch (c){
            case 'k': knn=atoi(optarg); break;
            case 'y': {
                int t = atoi(optarg);
                if (t < 0 || t > IndexType::LAST_TYPE) {
                    fprintf(stderr, "Index Type %d is not defined\n", t);
                }
                index_type = static_cast<IndexType>(t);
            }
                break;
            case 'i': filename = optarg; break;
            case 'q': queryFilename = optarg; break;
            case 'n': indexName= optarg; break;
            case 'c': nclusters=atoi(optarg); break;
            case 's': size=atoi(optarg); break;
            case 'd': dims=atoi(optarg); break;
            case 'r': radius = (float) atof(optarg); break;
            case ':': /// No operand
                fprintf(stderr,"Option -%c requires an operand\n", optopt);
                return -1;
            default: break;
        }
    }
    if (queryFilename.empty() || filename.empty()){
        printf("query filename or filename was empty");
        printf("filename: '%s'  queryfilename: '%s'", filename.c_str(), queryFilename.c_str());
        return -1;
    }

    printf("parms, type=%s, dim=%d, nclusters=%d, size=%d, knn=%d, radius=%f, filename=%s, qfilename=%s\n",
           typeToString(index_type).c_str(), dims, nclusters, size, knn, radius,filename.c_str(), queryFilename.c_str());

    Matrix<float> dataset;
    load_from_file(dataset, filename , indexName);

    Matrix<float> query;
    load_from_file(query, queryFilename, indexName);
    printf("loaded queryfile qs=%ld \tcols=%ld\n", query.rows, query.cols);
    mstat stat(dataset.rows, dataset.cols, dataset.ptr());
    for (size_t j = 0; j < dataset.cols; ++j) {
        printf("coord,dim=%ld\t mean=%f\t variance=%f\n", j, stat[j]->getMean(), stat[j]->getVariance());
    }

    printf("set\tsize=%ld\t AverageVariance=%f\t ", dataset.rows, stat.getAverageVariance());

    srand(1);

    IndexWrapper* pwrap = new IndexWrapper();
    pwrap->type = index_type;
    NNIndex<L2<float> >* pindex =0;
    switch(pwrap->type){
        case KDTREE:
            pindex = new KDTreeIndex<L2<float>>(dataset, KDTreeIndexParams(1));
            break;
        case FLANN_LSH:
            pindex = new flann::LshIndex<L2<float> >(dataset, flann::LinearIndexParams());
            break;
        default:
            break;
    }
    pwrap->pindex = pindex;

    if (knn > 0){
        calcKNNDistCalculations(pwrap, dataset, query, (size_t) knn);
    } else{
        calcRadiusDistCalculations(pwrap, dataset, query, radius);
    }

    printf("\n");
    delete[] query.ptr();
    delete[] dataset.ptr();

    return 0;
}

int main(int argc, char const * const argv[]) {
    signal(SIGSEGV, handler);
//    char const * const fakeArgs[] = {"",   "-i/Users/parnell/data/gaussian_nclus=5_dim=5_var=0.1_size=1000000.hdf5",
//                                    "-q/Users/parnell/data/queries/gaussian-query-1_nclus=5_dim=5_var=0.1_size=1000000.hdf5",
//                                    "-ngauss",
//                                    "-r0.4",
//                                    "-y1"};
//    argc = sizeof(fakeArgs)/sizeof(void*);
    if (argc < 3) {
        fprintf(stderr, "need at least 2 arguments\n");
        fprintf(stderr,
                "example:  -k3 -d5 -i/Users/parnell/workspace/data/gaussian_1_5_0.1_1000000.hdf5 -ngauss -f1 \n");
        return -1;
    }
    return run(argc, argv);
//    return run(argc, fakeArgs);
}
