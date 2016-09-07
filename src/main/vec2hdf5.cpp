#include <iostream>
#include <flann/flann.hpp>
#include <flann/io/hdf5.h>
#include <fstream> //ifstream
#include <sstream>

using flann::Matrix;
using std::vector;
using std::ifstream;
using std::cout;
using std::endl;

#include "vec2hdf5.h"

int main(int argc, char* argv[]) {
    if (argc < 4){
        fprintf(stderr,"./vec2hdf5 <infile> <outfile> <indexName>\n");
        return -1;
    }
    if (!strcmp(argv[1],argv[2])){
        fprintf(stderr,"infile and outfile can not be the same\n");
        return -1;
    }

    convertVecToHDF5<float>(argv[1], argv[2], argv[3], argc > 4);
    return 0;
}