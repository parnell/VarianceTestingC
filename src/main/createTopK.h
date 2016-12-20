//
//

#ifndef VARIANCETESTING_CREATETOPK_H
#define VARIANCETESTING_CREATETOPK_H
//#include <string>
//#include <lshbox/eval.h>
//#include <lshbox/matrix.h>
//#include <lshbox/topk.h>
//#include <lshbox/metric.h>

//template <typename DATATYPE>
//void createTopK(const lshbox::Matrix<DATATYPE>& data, std::string benchmark_file){
//    lshbox::Benchmark bench;
//    std::string benchmark(benchmark_file);
//    bench.load(benchmark);
//    unsigned K = bench.getK();
//
//    lshbox::Matrix<DATATYPE>::Accessor accessor(data);
//    lshbox::Metric<DATATYPE> metric((unsigned)data.getDim(), L1_DIST);
//
//    lshbox::Scanner<lshbox::Matrix<DATATYPE>::Accessor> scanner(
//            accessor,
//            metric,
//            K
//    );
//}

#endif //VARIANCETESTING_CREATETOPK_H
