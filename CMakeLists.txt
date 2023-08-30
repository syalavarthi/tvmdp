# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2023 Marvell.

# set CMake minimum version
cmake_minimum_required(VERSION 3.25)

# project name and useful settings
project(
  tvmdp
  VERSION 0.1.0
  DESCRIPTION "TVM Dataplane Library"
  LANGUAGES CXX C)

# set options
add_library(tvmdp_compiler_flags INTERFACE)

# set compile features
target_compile_features(tvmdp_compiler_flags INTERFACE cxx_std_17)

# add compiler warning flags
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,Clang,GNU>")
target_compile_options(tvmdp_compiler_flags INTERFACE
  "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wdeprecated;-Wunused;-Wundef;-Wuninitialized;-Wcast-qual;>>"
)

target_compile_options(tvmdp_compiler_flags INTERFACE
  "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wmissing-include-dirs;-Wmissing-declarations;-Wpointer-arith;-Wsign-compare>>"
)

target_compile_options(tvmdp_compiler_flags INTERFACE
  "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wformat=2;-Wformat-truncation=2;-Wformat-overflow=2;-Walloc-zero>>"
)

# add compiler optimization flags
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,Clang,GNU>")
target_compile_options(tvmdp_compiler_flags INTERFACE
  "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-O3;-march=armv8.5-a+sve2+crypto>>"
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
find_package(jansson QUIET)

if(jansson_FOUND)
  target_link_libraries(tvmdp_compiler_flags INTERFACE jansson::jansson)
else()
  include(FindPkgConfig)
  set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH ON)
  pkg_search_module(JANSSON REQUIRED jansson)
  include_directories(${JANSSON_INCLUDE_DIRS})
  target_link_libraries(tvmdp_compiler_flags INTERFACE ${JANSSON_LDFLAGS})
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
install(TARGETS tvmdp_compiler_flags EXPORT tvmdpTargets)

install(EXPORT tvmdpTargets
  NAMESPACE tvmdp::
  DESTINATION lib/cmake/tvmdp
)

configure_package_config_file(${PROJECT_SOURCE_DIR}/config/Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/tvmdpConfig.cmake
  INSTALL_DESTINATION lib/cmake/tvmdp
)

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/tvmdpConfigVersion.cmake
  COMPATIBILITY SameMajorVersion
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tvmdpConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/tvmdpConfigVersion.cmake
  DESTINATION lib/cmake/tvmdp
)

# generate pkgconfig
set(target tvmdp)
configure_file(${PROJECT_SOURCE_DIR}/config/tvmdp.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/tvmdp.pc @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tvmdp.pc
  DESTINATION lib/pkgconfig
)
