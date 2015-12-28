cmake_minimum_required(VERSION 3.3)
project(VarianceTesting)


find_package (HDF5)

include_directories(${HDF5_INCLUDE_DIRS})
set(_hdf5_libs hdf5 hdf5_cpp)


#link_directories(/usr/local/include/flann)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main.cpp src/vec2hdf5.h src/stats/fstat.cpp src/stats/fstat.h src/extras.h)
add_executable(VarianceTesting ${SOURCE_FILES})

#
target_link_libraries( VarianceTesting ${_hdf5_libs})
