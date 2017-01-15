
#ifndef VARIANCETESTING_BIN2VEC_H
#define VARIANCETESTING_BIN2VEC_H
#include <iostream>
#include <fstream> //ifstream
#include <sstream>
#include <vector>
#include <lshbox.h>

using std::ofstream;

template <typename T>
void convertBin2Vec(const char* ifn, const char* ofn, bool verbose=false){
    if (verbose)
        fprintf(stderr,"converting file %s\n", ifn);

    lshbox::Matrix<T> m;
    m.load(ifn);
    ofstream ofs(ofn);
    ofs << m.getSize() << " " << m.getDim() << " 2" << std::endl;
    T *pdata = m.getData();
    size_t dw = m.getDim();
    for (size_t i=0; i< m.getSize();++i){
        for (size_t j=0;j< m.getDim();++j){
            ofs << pdata[i*dw+j] << ' ';
        }
        ofs << std::endl;
    }
    ofs.close();
}

#endif //VARIANCETESTING_BIN2VEC_H
