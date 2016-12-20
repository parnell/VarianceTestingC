#include <iostream>
#include <fstream> //ifstream
#include <sstream>
#include <vector>

using std::vector;
using std::ifstream;
using std::ofstream;
using std::cout;
using std::endl;

#include <lshbox.h>
#include "vec2vect.h"

int main(int argc, char* argv[]) {
    if (argc < 3){
        fprintf(stderr,"./vec2vect <infile> <outfile>\n");
        return -1;
    }
    if (!strcmp(argv[1],argv[2])){
        fprintf(stderr,"infile and outfile can not be the same\n");
        return -1;
    }

    convertVec2Vect(argv[1], argv[2], argc > 3);
    return 0;
}
