# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2023 Marvell.

# enable docs build by default
option(ENABLE_DOCS "Build documentation" ON)

# check if Doxygen is installed
find_package(Doxygen)

if(DOXYGEN_FOUND)
    set(doxyfile_inp ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(doxyfile_out ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${doxyfile_inp} ${doxyfile_out} @ONLY)

    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile_out}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
else(DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
endif(DOXYGEN_FOUND)
