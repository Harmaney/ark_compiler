cmake_minimum_required(VERSION 3.1.0)
project(arkcompiler VERSION 0.1.0)

include(CTest)
enable_testing()

aux_source_directory(./src SOURCES)

add_executable(arkcompiler ${SOURCES})

set(CMAKE_CXX_STANDARD 17)
target_compile_features(arkcompiler PRIVATE cxx_std_17)
include(CPack)
