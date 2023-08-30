# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2023 Marvell.

# library header files list
set(HEADER_LIST_LIBRARY
  "${PROJECT_SOURCE_DIR}/include/tvmdp.h"
)

# library source files list
set(SOURCE_LIST_LIBRARY
  "${PROJECT_SOURCE_DIR}/src/tvmdp.cpp"
)

# automatic library - will be static or dynamic based on user setting
add_library(tvmdp
  ${SOURCE_LIST_LIBRARY}
  ${HEADER_LIST_LIBRARY}
)

set_target_properties(tvmdp PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(tvmdp PROPERTIES SOVERSION 0)

target_compile_definitions(tvmdp PRIVATE DMLC_USE_LOGGING_LIBRARY="tvm/runtime/logging.h")

# include directories
target_include_directories(tvmdp PUBLIC ${PROJECT_SOURCE_DIR}/include)

# link base flags
target_link_libraries(tvmdp PUBLIC tvmdp_compiler_flags)

# state that tvmdpPIC when the default is shared libraries
set_target_properties(tvmdp PROPERTIES POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})

# additional properties
set_property(TARGET tvmdp PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET tvmdp PROPERTY VERSION ${tvmdp_VERSION_MAJOR}.${tvmdp_VERSION_MINOR}.${tvmdp_VERSION_PATCH})
set_property(TARGET tvmdp PROPERTY SOVERSION ${tvmdp_VERSION_MAJOR})

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST_LIBRARY}
)

# install headers
install(FILES ${HEADER_LIST_LIBRARY} DESTINATION include)

# install libs
install(TARGETS tvmdp DESTINATION lib)
