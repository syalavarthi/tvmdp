# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2023 Marvell.

# set CMake minimum version
cmake_minimum_required(VERSION 3.25)

# project name and useful settings
project(
  libtvmdp
  VERSION 0.1.0
  DESCRIPTION "TVM Dataplane Library"
  LANGUAGES CXX)

# set options
add_library(tvmdp_compiler_flags INTERFACE)

# set compile features
target_compile_features(tvmdp_compiler_flags INTERFACE cxx_std_17)

# add compiler warning flags
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,Clang,GNU>")
target_compile_options(tvmdp_compiler_flags INTERFACE
  "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
)

# ensure -std=c++xx instead of -std=g++xx
set(CMAKE_CXX_EXTENSIONS OFF)

# support folders in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# skip RPATH
set(CMAKE_SKIP_BUILD_RPATH TRUE)

# enable testing
include(CTest)

# find dmlc headers
find_package(dmlc REQUIRED)

# find dlpack headers
find_package(dlpack REQUIRED)

# find jansson library and headers, update flags
find_package(jansson REQUIRED)

if(jansson_FOUND)
  target_link_libraries(tvmdp_compiler_flags INTERFACE jansson::jansson)
else()
  message(STATUS "Jansson not found, Abort")
endif()

# find tvm library and runtime headers, update flags
find_package(tvm REQUIRED)

if(tvm_FOUND)
  target_link_libraries(tvmdp_compiler_flags INTERFACE
    -Wl,--whole-archive,$<TARGET_FILE:tvm::tvm_runtime>,--no-whole-archive)
else()
  message(STATUS "TVM not found, Abort")
endif()

# find Pthreads library
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(tvmdp_compiler_flags INTERFACE Threads::Threads)

# Check for library with dlopen
target_link_libraries(tvmdp_compiler_flags INTERFACE ${CMAKE_DL_LIBS})

# library source code
add_subdirectory(src)

# unit tests
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

# documentation
add_subdirectory(docs)

# cmake package config generation
include(CMakePackageConfigHelpers)
install(TARGETS tvmdp_compiler_flags EXPORT libtvmdpTargets)

install(EXPORT libtvmdpTargets
  NAMESPACE libtvmdp::
  DESTINATION lib/cmake/libtvmdp
)

configure_package_config_file(${PROJECT_SOURCE_DIR}/config/Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/libtvmdpConfig.cmake
  INSTALL_DESTINATION lib/cmake/libtvmdp
)

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/libtvmdpConfigVersion.cmake
  COMPATIBILITY SameMajorVersion
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libtvmdpConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/libtvmdpConfigVersion.cmake
  DESTINATION lib/cmake/libtvmdp
)

# generate pkgconfig
set(target tvmdp)
configure_file(${PROJECT_SOURCE_DIR}/config/libtvmdp.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/libtvmdp.pc @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libtvmdp.pc
  DESTINATION lib/pkgconfig
)
