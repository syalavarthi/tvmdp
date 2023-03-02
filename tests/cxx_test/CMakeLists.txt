# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2023 Marvell.

# tests need to be added as executables first
add_executable(tvmdp_test_cxx tvmdp_test.cpp)

# link with main library
target_link_libraries(tvmdp_test_cxx PRIVATE tvmdp)
target_link_libraries(tvmdp_test_cxx PUBLIC tvmdp_compiler_flags)

# include  directories
target_include_directories(tvmdp_test_cxx PUBLIC ${PROJECT_SOURCE_DIR}/include)

# register a test with ctest
add_test(NAME tvmdp_basic_test_cxx COMMAND tvmdp_test_cxx)

# install test binaries
install(TARGETS tvmdp_test_cxx DESTINATION bin)
