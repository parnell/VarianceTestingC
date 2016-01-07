#include <iostream>
#include <vector>

#include <flann/flann.hpp>
#include <flann/io/hdf5.h>
#include "vec2hdf5.h"
#include "stats/fstat.h"
#include "extras.h"


using namespace std;
using namespace flann;


enum IndexType{
    UNKNOWN,
    KDTREE,
    LSH
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
                break;
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

    Matrix<int> result_indices(new int[query.rows*nn], query.rows, nn);
    Matrix<float> dists(new float[query.rows*nn], query.rows , nn); /// distances of the knn results

    utime64 start = localClock();
    pindex->buildIndex();
    utime64 end = localClock();

    pwrap->resetCalcs();
    pindex->knnSearch(query, result_indices, dists, nn, SearchParams(-1));
    utime64 qend = localClock();
    size_t dc = pwrap->getCalcs();


    printf("Ending KNN, buildtime=%f, querytime=%f, avgquerytime=%f, knn=%ld, queries=%ld, totalcalcs=%ld, avg=%f\n",
           (double)(end - start)/1000000,
           (double)(qend - end)/1000000,
           (double)(qend - end)/1000000/query.rows,
           nn, query.rows, dc, (float)dc / query.rows );

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


int run(int argc, char const * const argv[]) {
    cout << "Starting argc=" << argc << endl;
    for (int k = 0; k < argc; ++k) {
        cout << argv[k] << " ";
    }
    cout << endl;
    string filename;
    string queryFilename;

    float radius = 0.4;
    const char *indexName = 0;
    int c;
    int knn = -1;
    int index_type = KDTREE;


    int dims=-1;
    int nclusters = -1;
    int size = -1;
    while((c =  getopt(argc, const_cast<char * const * >(argv), "rk:y:c:s:d:r:i:q:n:")) != EOF){
        switch (c){
            case 'k': knn=atoi(optarg); break;
            case 'y': index_type=atoi(optarg); break;
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

    printf("parms, dim=%d, nclusters=%d, size=%d, knn=%d, radius=%f, filename=%s, qfilename=%s\n",
           dims, nclusters, size, knn, radius,filename.c_str(), queryFilename.c_str());

    Matrix<float> dataset;
    load_from_file(dataset, filename.c_str() , indexName);

    Matrix<float> query;
    load_from_file(query, queryFilename.c_str() , indexName);

    mstat stat(dataset.rows, dataset.cols, dataset.ptr());
    for (size_t j = 0; j < dataset.cols; ++j) {
        printf("coord,dim=%ld, mean=%f, variance=%f\n", j, stat[j]->getMean(), stat[j]->getVariance());
    }

    printf("set,size=%ld, AverageVariance=%f, ", dataset.rows, stat.getAverageVariance());
    string dir("/Users/parnell/workspace/data");

    srand(1);
    stringstream qss;
    qss << dir << "gaussian_query" << nclusters << "_" << dims << "_" << size << "." <<".hdf5";
    IndexWrapper* pwrap = new IndexWrapper();
    pwrap->type = index_type==KDTREE? KDTREE : LSH;
    NNIndex<L2<float> >* pindex =0;
    switch(pwrap->type){
        case KDTREE:
            pindex = new KDTreeIndex<L2<float>>(dataset, KDTreeIndexParams(1));
            break;
        case LSH:
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

    stringstream ss;

    string dir("/Users/parnell/workspace/data");
    int nclusters = 1;
    int dims = 5;
    int size = 1000000;
    float variance = 0.1;
    ss << dir << "/gaussian_" << nclusters << "_" << dims << "_" << variance << "_" << size << ".hdf5";


    stringstream iss;
    iss << "-i" << ss.str();

    if (argc < 3) {
        fprintf(stderr, "need at least 2 arguments\n");
        fprintf(stderr, "example:  -k3 -d5 -i/Users/parnell/workspace/data/gaussian_1_5_0.1_1000000.hdf5 -ngauss -f1 \n");
    }
    return run(argc, argv);


//    argc=4; char const * const fakeArgs[] = {"",   "-i/Users/parnell/data/gaussian_nclus=5_dim=5_var=0.1_size=1000000.hdf5",
//                                    "-q/Users/parnell/data/queries/gaussian-query-1_nclus=5_dim=5_var=0.1_size=1000000.hdf5",
//                                    "-ngauss",
//                                    "-r0.4"};
//    return run(argc, fakeArgs);
}