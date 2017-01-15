//
//
//

#include "verifylshbin.h"

#include <string>

#include <lshbox.h>
#include <unistd.h>

using std::string;
using namespace lshbox;


int main(int argc, char *argv[])
{

    string data_file(argv[1]);

    int c; /// character for getopt
    int M=500; ///(521) Hash table size
    int L=5; /// Number of hash tables
    int S = 100; /// Size of vectors in train
    int I = 50; /// Training iterations
    int N = -1; /// Binary code bytes

    while((c =  getopt(argc, argv, "i:")) != EOF){
        switch (c){
            case 'i': data_file = optarg; break;
            case ':': /// No operand
                fprintf(stderr,"Option -%c requires an operand\n", optopt);
                return -1;
            default: break;
        }
    }

    if (argc > 2)
    {
        std::cerr << "Usage: ./verifylshbin <input file>" << std::endl;
        return -1;
    }

    std::cout << "#LOADING DATA ... <" << data_file << ">" << std::endl;
    lshbox::timer timer;
    lshbox::Matrix<float> data(data_file);

    std::cout << "dataloadtime= " << timer.elapsed() << " (s)\tsize=" << data.getSize() << "\tdim=" << data.getDim() <<std::endl;

}