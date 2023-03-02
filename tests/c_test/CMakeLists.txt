# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2023 Marvell.

# tests need to be added as executables first
add_executable(tvmdp_test_c tvmdp_test.c)

# link with main library
target_link_libraries(tvmdp_test_c PRIVATE tvmdp)
target_link_libraries(tvmdp_test_c PUBLIC tvmdp_compiler_flags)

# include  directories
target_include_directories(tvmdp_test_c PUBLIC ${PROJECT_SOURCE_DIR}/include)

# use C linker
set_target_properties(tvmdp_test_c PROPERTIES LINKER_LANGUAGE C)

# register a test with ctest
add_test(NAME tvmdp_basic_test_c COMMAND tvmdp_test_c)

# install test binaries
install(TARGETS tvmdp_test_c DESTINATION bin)
