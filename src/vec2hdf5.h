//
// Created by Lee Parnell Thompson on 12/1/15.
//

#ifndef VARIANCETESTING_VEC2HDF5_H_H
#define VARIANCETESTING_VEC2HDF5_H_H

#include <flann/flann.hpp>
#include <flann/io/hdf5.h>
#include <fstream> //ifstream
#include <vector>
#include <iostream>
#include <sstream>
#include <string>

using flann::Matrix;
using std::vector;
using std::ifstream;
using std::cout;
using std::endl;

void convertVecToHDF5(const char* ifn, const char* ofn,const char* ofn2, bool verbose=false){
    ifstream ifs(ifn);
    if (!ifs.is_open()){
        fprintf(stderr,"error opening file %s", ifn);
        return;
    }
    if (verbose)
        cout << "Converting file " << ifn << endl;
    std::string line;
    std::getline(ifs,line);
    std::istringstream iss(line);
    size_t dim, size, l;
    iss >> dim >> size >> l;

    float f;
    if (verbose)
        cout << "Dim " << dim << "  size " << size << " l=" << l << endl;

    vector<float> vec(dim*size);
    int i = -1;
    while(ifs >> f){
        vec[++i] = f;
    }

    Matrix<float> data(&vec[0],size, dim);

    flann::save_to_file(data, ofn,ofn2);
}




#endif //VARIANCETESTING_VEC2HDF5_H_H
