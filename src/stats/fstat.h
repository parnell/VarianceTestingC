//
// Created by Lee Parnell Thompson
//

#ifndef VARIANCETESTING_FSTAT_H
#define VARIANCETESTING_FSTAT_H

#include <cfloat>
#include <algorithm>
#include <cmath>
#include <iostream>
using namespace std;

class fstat {
public:
    size_t count;
    float stat[4];
    bool dirty;
    float var;
    float mean;
    fstat() : dirty(true), count(0), stat{0,0,FLT_MAX,FLT_MIN}{

    }

    void addStat(float val){
        ++count;
        stat[0] += val;
        stat[1] += val*val;
        stat[2] = std::min(stat[2], val);
        stat[3] = std::max(stat[3], val);
        dirty=true;
    }

    inline float getMean(){
        return stat[0]/count;
    }

    float getDeviation(){
        return (1.0/count) * std::sqrt(count*stat[1]- stat[0]*stat[0]);
    }

    float getVariance(){
        if (dirty){
            float f = getDeviation();
            var = f*f;
            dirty = false;
        }
        return var;
    }
};


class mstat {
public:
    fstat* props;
    size_t rows, cols;

    mstat(size_t rows, size_t cols, float* ptr) : rows(rows), cols(cols){
        props = new fstat[cols];
        for(size_t i = 0;i<rows*cols;++i){
            props[i % cols].addStat(*ptr++);
        }
    }

    float getAverageVariance(){
        float avg = 0;
        for (size_t i = 0; i < cols; ++i) {
            avg += props[i].getVariance();
//            cout << " i = " << i << "   " << props[i].getMean() << "   " << props[i].getVariance() << endl;
        }
        return avg/cols;
    }
    inline fstat* operator[](size_t index) const{
        return &props[index];
    }
};
#endif //VARIANCETESTING_FSTAT_H
