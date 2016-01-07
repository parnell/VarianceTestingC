#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <vector>

#include <flann/flann.hpp>
#include <flann/io/hdf5.h>
#include <fstream> //ifstream
#include "vec2hdf5.h"
#include "stats/fstat.h"
#include <iterator>
#include <unistd.h> //getopt

Matrix<float> generateQuery(Matrix<float> matrix,size_t querySize);

using namespace std;
using namespace flann;



enum IndexType{
    KDTREE,
    LSH
};


void saveAsVec(Matrix<float> m, const char* fn){
    ofstream of;
    of.open(fn);
    of << m.cols << " " << m.rows<< " " << " 2" << endl;
    size_t i,j;
    for (i = 0; i < m.rows; ++i) {
        for (j = 0; j < m.cols; ++j) {
            of << m[i][j];
            if (j != m.cols-1) {
                of << " ";
            }
        }
        of << endl;
    }
    of.close();
}

int calcKNNDistCalculations(const Matrix<float>& dataset,const Matrix<float>& query, const size_t nn){

    Matrix<int> result_indices(new int[query.rows*nn], query.rows, nn);
    Matrix<float> dists(new float[query.rows*nn], query.rows , nn); /// distances of the knn results

    KDTreeIndex<L2<float>> index(dataset, KDTreeIndexParams(1));

    index.buildIndex();
    // knn search 128 checks
    flann::L2<float>.distance_calcs=0;
    index.knnSearch(query, result_indices, dists, nn, SearchParams(-1));

    size_t dc = flann::L2<float>.distance_calcs;
    cout << "Ending KNN, knn=" << nn <<", queries="<<query.rows <<" ,totalcalcs=" << flann::L2<float>.distance_calcs << ", avg=" << index.distance_calcs / query.rows << endl;
    /// cleanup
    delete[] result_indices.ptr();
    delete[] dists.ptr();
    return 0;
}

int calcRadiusDistCalculations(const Matrix<float>& dataset,const Matrix<float>& query, const float radius){

    vector< vector<size_t> > result_indices;
    vector< vector<float> > dists;
//    Matrix<int> result_indices(new int[query.rows*nn], query.rows, nn);
//    Matrix<float> dists(new float[query.rows*nn], query.rows , nn); /// distances of the knn results

    KDTreeIndex<L2<float>> index(dataset, KDTreeIndexParams(1));

    index.buildIndex();
    // knn search 128 checks
    index.distance_calcs=0;
    flann::L2<float>::distance_calcs=0;
    index.radiusSearch(query, result_indices, dists, radius, SearchParams(-1));

    size_t nresults =0;
    for (size_t i = 0; i < result_indices.size(); ++i) {
        nresults += result_indices[i].size();
    }
    printf("radius%f, queries=%ld, totalcalcs=%ld, avg=%f, size=%zu, avgresults=%f\n",
           radius,
           query.rows,
           index.distance_calcs ,
           (float) index.distance_calcs / query.rows,
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
//    bool saveFiles = false;

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
    if (index_type == KDTREE){
        if (knn > 0){
            calcKNNDistCalculations(dataset, query, (size_t) knn);
        } else{
            calcRadiusDistCalculations(dataset, query, radius);
        }
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


//  argc=3; char * fakeArgs[] = {"/Users/parnell/workspace/data/gaussian-20-10-1000000.txt", "/Users/parnell/workspace/data/gaussian-20-10-1000000.hdf5","gaussian"};
//    argc=3; char * fakeArgs[] = {"/Users/parnell/workspace/data/gaussian-1-10-1000000.vec", "/Users/parnell/workspace/data/gaussian-1-10-1000000.hdf5","gaussian"};
//    argc=3; char const * const fakeArgs[] = {   "-i/Users/parnell/workspace/data/gaussian_1_5_0.4_1000000.hdf5",
//                                    "-q/Users/parnell/workspace/data/gaussian_query_1_5_0.4_100.vec",
//                                    "-ngaussian",
//                                    "-r0.4"};
//    return convert(argc, fakeArgs);
//    argc = 6;  char const * const fakeArgs[] = {argv[0],"-k3", "-d5", iss.str().c_str(), "-ngauss", "-f1"};
//    argc = 2; char * fakeArgs[] = {"/Users/parnell/workspace/data/gaussian-1-10-1000000.hdf5", "gaussian"};
//    char size[] = "1000000";
//    std::stringstream sstm;
//    sstm << "/Users/parnell/workspace/data/gaussian_1_5_0.1_" << size << ".hdf5";

//    argc = 2; char * fakeArgs[] = {"/Users/parnell/workspace/data/gaussian_1_5_0.1_1000000.hdf5", "gauss"};

//    return run(argc, fakeArgs);

    return run(argc, argv);
}