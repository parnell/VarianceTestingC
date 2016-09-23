cmake_minimum_required(VERSION 3.3)
project(VarianceTesting)


find_package (HDF5)

include_directories(${HDF5_INCLUDE_DIRS})
set(_hdf5_libs hdf5 hdf5_cpp)

include_directories(/Users/parnell/src/LSHBOX/include)


link_directories(/usr/local/include/flann)
link_directories(/usr/local/Cellar/hdf5/1.8.16/lib)
link_directories(/Users/parnell/src/LSHBOX/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(PROGRAMS VarianceTesting CreateLSHBenchmark vec2bin vec2hdf5 LSHBox)

set(SOURCE_FILES src/main/vec2hdf5.h src/main/stats/fstat.cpp src/main/stats/fstat.h src/main/extras.h src/main/vec2bin.h src/test/TestLSH.cpp src/test/TestLSH.h)

foreach(program ${PROGRAMS})
    add_executable(${program} ${SOURCE_FILES} src/main/${program}.cpp)
    target_link_libraries( ${program} ${_hdf5_libs})
endforeach(program)

#
