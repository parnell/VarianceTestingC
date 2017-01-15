
#include "bin2vec.h"

int main(int argc, char* argv[]) {
    if (argc < 3){
        fprintf(stderr,"./bin2vec <infile> <outfile>\n");
        return -1;
    }
    if (!strcmp(argv[1],argv[2])){
        fprintf(stderr,"infile and outfile can not be the same\n");
        return -1;
    }

    convertBin2Vec<float>(argv[1], argv[2], argc > 3);
    return 0;
}
