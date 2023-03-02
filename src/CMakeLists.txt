# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2023 Marvell.

# library header files list
set(HEADER_LIST_LIBRARY
  "${PROJECT_SOURCE_DIR}/include/tvmdp.hpp"
)

# c wrapper header files list
set(HEADER_LIST_WRAPPER
  "${PROJECT_SOURCE_DIR}/include/tvmdp.h"
)

# library source files list
set(SOURCE_LIST_LIBRARY
  "${PROJECT_SOURCE_DIR}/src/tvmdp.cpp"
)

# c wrapper source files list
set(SOURCE_LIST_WRAPPER
  "${PROJECT_SOURCE_DIR}/src/tvmdp_c.cpp"
)

# automatic library - will be static or dynamic based on user setting
add_library(tvmdp
  ${SOURCE_LIST_LIBRARY}
  ${HEADER_LIST_LIBRARY}
  ${HEADER_LIST_WRAPPER}
  ${SOURCE_LIST_WRAPPER}
)

target_compile_definitions(tvmdp PRIVATE DMLC_USE_LOGGING_LIBRARY="tvm/runtime/logging.h")

# include directories
target_include_directories(tvmdp PUBLIC ${PROJECT_SOURCE_DIR}/include)

# link base flags
target_link_libraries(tvmdp PUBLIC tvmdp_compiler_flags)

# state that libtvmdp need PIC when the default is shared libraries
set_target_properties(tvmdp PROPERTIES POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})

# additional properties
set_property(TARGET tvmdp PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET tvmdp PROPERTY VERSION ${libtvmdp_VERSION_MAJOR}.${libtvmdp_VERSION_MINOR}.${libtvmdp_VERSION_PATCH})
set_property(TARGET tvmdp PROPERTY SOVERSION ${libtvmdp_VERSION_MAJOR})

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST_LIBRARY} ${HEADER_LIST_WRAPPER}
)

# install headers
set(installable_headers ${HEADER_LIST_LIBRARY} ${HEADER_LIST_WRAPPER})
install(FILES ${installable_headers} DESTINATION include)

if(BUILD_SHARED_LIBS)
  # install libs
  install(TARGETS tvmdp DESTINATION lib)
else()
  # rename target
  set_target_properties(tvmdp PROPERTIES OUTPUT_NAME "libtvmdp_simple")

  # generate combined library
  set(LIBTVMDP "libtvmdp.a")
  add_custom_command(
    OUTPUT ${LIBTVMDP}
    COMMAND ${CMAKE_AR} -rcT ${LIBTVMDP} $<TARGET_FILE:tvm::tvm_runtime> $<TARGET_FILE:jansson::jansson> $<TARGET_FILE:tvmdp>
    DEPENDS tvmdp
    COMMENT "Generating combined library"
  )

  add_custom_target(tvmdp_combined ALL
    DEPENDS ${LIBTVMDP}
  )

  # install libs - combined library
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${LIBTVMDP} DESTINATION lib)
endif()
