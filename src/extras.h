//
// Created by Lee Parnell Thompson on 12/16/15.
//

#ifndef VARIANCETESTING_EXTRAS_H
#define VARIANCETESTING_EXTRAS_H


#include <flann/flann.hpp>
#include <flann/io/hdf5.h>
#include <fstream> //ifstream
#include "vec2hdf5.h"
#include "stats/fstat.h"
#include <iterator>
#include <sys/time.h>
typedef unsigned long long int utime64;

inline utime64 localClock(){
    struct timeval tv;
    gettimeofday(&tv,0);
    return (((utime64)tv.tv_sec) *1000000 + tv.tv_usec) *1000;
}
//
//int runRemovalPartition(int argc, char** argv) {
//    cout << "Starting" << endl;
//    bool saveFiles = false;
//    size_t nn = 1;
//    const float radius = 0.4;
//    const char *fn = argv[0];
//    const char *indexName = argv[1];
//
//    srand(1);
//
//    Matrix<float> dataset;
//    vector<float> newd;
//
//    load_from_file(dataset, fn , indexName);
//    mstat stat(dataset.rows, dataset.cols, dataset.ptr());
//    for (size_t j = 0; j < dataset.cols; ++j) {
//        printf("dim=%ld, mean=%f, variance=%f\n", j, stat[j]->getMean(), stat[j]->getVariance());
//    }
//
//    printf("size=%ld, Average Variance=%f\n", dataset.rows, stat.getAverageVariance());
//
//    const int numsteps = 10;
//    float maxt = stat.getAverageVariance()*4;
//    float stept = maxt / numsteps;
//    Matrix<float> query = generateQuery(dataset, 100);
////    saveAsVec(query, "/Users/parnell/workspace/data/gaussian_query-1-1-100.vec");
////return -1;
//    for (int i = 0; i < numsteps+1; ++i) {
//        float tau = stept * i;
//
//        for (size_t r = 0; r < dataset.rows; ++r) {
//            float *ppoint = dataset[r];
//            bool within = true;
//            for (size_t j = 0; j < dataset.cols; ++j) {
////            printf("r=%ld   ppoint[j]=%f  f[j]=%f   fabs=%f\n",r, ppoint[j], stat[j]->getMean(), fabs(ppoint[j] - stat[j]->getMean() ));
//                if (fabs(ppoint[j] - stat[j]->getMean()) > tau) {
//                    within = false;
//                    break;
//                }
//            }
//            if (!within) {
//                for (size_t j = 0; j < dataset.cols; ++j) {
//                    newd.push_back(ppoint[j]);
//                }
//            }
//        }
//        mstat nstat(newd.size() / dataset.cols, dataset.cols, &newd[0]);
//        Matrix<float> newMatrix(&newd[0], newd.size() / dataset.cols, dataset.cols);
//        if (saveFiles) {
//            std::stringstream sstm;
//            sstm << "/Users/parnell/workspace/data/gaussian-1-1-1000000.tau=" << tau << ".vec";
//            saveAsVec(newMatrix, sstm.str().c_str());
////        save_to_file(newMatrix,sstm.str().c_str() , "gauss");
//        }
//
//        printf("Tau = %f, excluded#=%ld, newMatrixSize=%ld, newMatrixVariance=%f,", tau, dataset.rows - newMatrix.rows,newMatrix.rows, nstat.getAverageVariance());
//        calcRadiusDistCalculations(newMatrix, query, radius);
////        calcKNNDistCalculations(newMatrix, query, nn);
//        newd.clear();
//    }
//    delete[] dataset.ptr();
//    delete[] query.ptr();
//
//    return 0;
//}



#endif //VARIANCETESTING_EXTRAS_H
