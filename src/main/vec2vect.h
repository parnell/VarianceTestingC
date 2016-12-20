
#ifndef VARIANCETESTING_VEC2VECT_H
#define VARIANCETESTING_VEC2VECT_H

void convertVec2Vect(const char* ifn, const char* ofn, bool verbose=false){
    ifstream ifs(ifn);
    ofstream ofs(ofn);
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

    ofs << ifs.rdbuf();
    ifs.close();
    ofs.close();
}

#endif //VARIANCETESTING_VEC2VECT_H
