# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2023 Marvell.

# set cmake minimum version
cmake_minimum_required(VERSION 3.24)

# project name and useful settings
project(
  tvmdp
  VERSION 1.0.0
  DESCRIPTION "TVM Dataplane Library"
  LANGUAGES CXX C)

# set project options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

# enable all warnings
add_compile_options(-Wall -Wextra -Wpedantic)

# public headers list
set(TVMDP_HEADERS
  "${PROJECT_SOURCE_DIR}/include/tvmdp.h"
)

# sources
set(TVMDP_SOURCES
  "${PROJECT_SOURCE_DIR}/src/tvmdp.cpp"
)

# define libtvmdp
add_library(tvmdp ${TVMDP_SOURCES} ${TVMDP_HEADERS})

# include directories
target_include_directories(tvmdp PRIVATE ${PROJECT_SOURCE_DIR}/include)

# additional properties
set_property(TARGET tvmdp PROPERTY VERSION ${tvmdp_VERSION_MAJOR}.${tvmdp_VERSION_MINOR}.${tvmdp_VERSION_PATCH})
set_property(TARGET tvmdp PROPERTY SOVERSION ${tvmdp_VERSION_MAJOR})

# find jansson library, use pkg-config as fallback
find_package(jansson QUIET)
if(jansson_FOUND)
  target_link_libraries(tvmdp PUBLIC jansson::jansson)
else()
  include(FindPkgConfig)
  set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH ON)
  pkg_search_module(JANSSON REQUIRED jansson)
  include_directories(${JANSSON_INCLUDE_DIRS})
  target_link_libraries(tvmdp PUBLIC ${JANSSON_LDFLAGS})
endif()

# find dlpack, headers only
find_package(dlpack REQUIRED)
target_link_libraries(tvmdp PUBLIC dlpack::dlpack)

# find dmlc, headers only
find_package(OpenMP)
find_package(dmlc REQUIRED)
target_link_libraries(tvmdp PUBLIC dmlc::dmlc)

# find tvm
find_package(tvm REQUIRED)
target_link_libraries(tvmdp PUBLIC tvm::tvm_runtime)

# link with libdl
target_link_libraries(tvmdp PUBLIC ${CMAKE_DL_LIBS})

# setup standard installation directories
include(GNUInstallDirs)

# install headers
install(
  FILES ${TVMDP_HEADERS}
  DESTINATION include)

# install libraries
install(
  TARGETS tvmdp
  EXPORT tvmdpTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib)

install(
  EXPORT tvmdpTargets
  FILE tvmdpTargets.cmake
  NAMESPACE tvmdp::
  DESTINATION lib/cmake/tvmdp)

# install package configurations
include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/config/tvmdp-config.cmake.in
  ${CMAKE_BINARY_DIR}/cmake/tvmdp-config.cmake
  INSTALL_DESTINATION lib/cmake/tvmdp)
write_basic_package_version_file(
  ${CMAKE_BINARY_DIR}/cmake/tvmdp-config-version.cmake
  VERSION ${TVMDP_VERSION}
  COMPATIBILITY AnyNewerVersion)
install(
  FILES ${CMAKE_BINARY_DIR}/cmake/tvmdp-config.cmake ${CMAKE_BINARY_DIR}/cmake/tvmdp-config-version.cmake
  DESTINATION lib/cmake/tvmdp)

# generate pkg-config
set(target tvmdp)
configure_file(
  ${PROJECT_SOURCE_DIR}/config/tvmdp.pc.in
  ${CMAKE_CURRENT_BINARY_DIR}/tvmdp.pc @ONLY)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/tvmdp.pc
  DESTINATION lib/pkgconfig)

# documentation
add_subdirectory(docs)
