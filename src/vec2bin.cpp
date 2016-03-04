#include <iostream>
#include <fstream> //ifstream
#include <sstream>
#include <vector>

using std::vector;
using std::ifstream;
using std::cout;
using std::endl;

#include <lshbox.h>
#include "vec2bin.h"
int main(int argc, char* argv[]) {
    if (argc < 3){
        fprintf(stderr,"./vec2bin <infile> <outfile>\n");
        return -1;
    }
    if (!strcmp(argv[1],argv[2])){
        fprintf(stderr,"infile and outfile can not be the same\n");
        return -1;
    }

    convertVec2Bin<float>(argv[1], argv[2], argc > 3);
    return 0;
}
