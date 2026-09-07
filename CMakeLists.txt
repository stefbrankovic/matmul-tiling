cmake_minimum_required(VERSION 3.16)
project(matmul_tiling CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# -O2 je podrazumevano za glavno poredjenje.
# -O3 -march=native se meri posebno:  cmake -B build-fast -DAGGRESSIVE=ON
option(AGGRESSIVE "Use -O3 -march=native" OFF)
if(AGGRESSIVE)
  set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")
else()
  set(CMAKE_CXX_FLAGS_RELEASE "-O2")
endif()

find_package(OpenMP)
find_package(BLAS)

add_executable(matmul
  src/main.cpp
  src/timing.cpp
  src/verify.cpp
  src/bandwidth.cpp
  src/kernels/naive.cpp
  src/kernels/reorder.cpp
  src/kernels/tiled.cpp
  src/kernels/parallel.cpp
  src/kernels/reference.cpp
  src/kernels/transposed.cpp)

target_include_directories(matmul PRIVATE include)

if(OpenMP_CXX_FOUND)
  target_link_libraries(matmul PRIVATE OpenMP::OpenMP_CXX)
  message(STATUS "OpenMP: ON")
else()
  message(STATUS "OpenMP: OFF")
endif()

if(BLAS_FOUND)
  target_link_libraries(matmul PRIVATE ${BLAS_LIBRARIES})
  target_compile_definitions(matmul PRIVATE HAVE_BLAS)
  message(STATUS "BLAS: ${BLAS_LIBRARIES}")
else()
  message(STATUS "BLAS: OFF")
endif()

message(STATUS "Flags: ${CMAKE_CXX_FLAGS_RELEASE}")
