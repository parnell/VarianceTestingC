cmake_minimum_required(VERSION 3.3)
project(VarianceTesting)

set(TTYPE "TACC")
message("Running cmake with target type ${TTYPE}")
find_package (HDF5)

include_directories(${HDF5_INCLUDE_DIRS})
set(_hdf5_libs hdf5 hdf5_cpp)

include_directories($ENV{HOME}/src/LSHBOX/include)

link_directories(/usr/local/include/flann)
if("${TTYPE}" STREQUAL "TACC")
	link_directories($ENV{TACC_HDF5_LIB})
else()
	link_directories(/usr/local/Cellar/hdf5/1.8.16/lib)
	link_directories(/Users/parnell/src/LSHBOX/include)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(PROGRAMS VarianceTesting CreateLSHBenchmark vec2bin vec2hdf5 vec2vect LSHBox)
set(NOHDF5PROGRAMS verifylshbin bin2vec)

set(SOURCE_FILES src/main/vec2hdf5.h src/main/stats/fstat.cpp src/main/stats/fstat.h
        src/main/extras.h src/main/vec2bin.h src/main/vec2vect.h src/test/TestLSH.cpp
        src/test/TestLSH.h src/main/createTopK.cpp src/main/createTopK.h )

set(NOHDF5SOURCE_FILES src/main/verifylshbin.h src/main/bin2vec.h src/main/flann_dist_revised.h.h)

foreach(program ${PROGRAMS})
    add_executable(${program} ${SOURCE_FILES} src/main/${program}.cpp)
    target_link_libraries( ${program} ${_hdf5_libs})
endforeach(program)

foreach(program ${NOHDF5PROGRAMS})
    add_executable(${program} ${NOHDF5SOURCE_FILES} src/main/${program}.cpp)
endforeach(program)

#
