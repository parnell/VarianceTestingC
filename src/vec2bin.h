
#ifndef VARIANCETESTING_VEC2BIN_H
#define VARIANCETESTING_VEC2BIN_H


template <typename T>
void convertVec2Bin(const char* ifn, const char* ofn, bool verbose=false){
    ifstream ifs(ifn);
    if (!ifs.is_open()){
        fprintf(stderr,"error opening file %s\n", ifn);
        return;
    }
    if (verbose)
        fprintf(stderr,"converting file %s\n", ifn);

    std::string line;
    std::getline(ifs,line);
    std::istringstream iss(line);
    size_t dim, size, l;
    iss >> dim >> size >> l;

    if (verbose)
        fprintf(stderr,"dim=%ld, size=%ld, l=%ld\n", dim,size,l);

    vector<T> vec(dim*size);
    float f;
    int i = -1;
    while(ifs >> f){
        vec[++i] = f;
    }
    lshbox::Matrix<T> m;
    m.load(vec, static_cast<int>(size),static_cast<int>(dim));
    m.save(ofn);
}

#endif //VARIANCETESTING_VEC2BIN_H
