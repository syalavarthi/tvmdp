# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2023 Marvell.

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_Fortran_COMPILER aarch64-linux-gnu-gfortran CACHE FILEPATH "GNU Fortran compiler")
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++ CACHE FILEPATH "GNU C++ compiler")
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc CACHE FILEPATH "GNU C compiler")
set(CMAKE_LINKER aarch64-linux-gnu-ld CACHE FILEPATH "GNU linker")
set(CMAKE_AR aarch64-linux-gnu-ar CACHE FILEPATH "GNU archiver")
set(CMAKE_RANLIB aarch64-linux-gnu-ranlib CACHE FILEPATH "GNU ranlib")
set(CMAKE_ADDR2LINE aarch64-linux-gnu-addr2line CACHE FILEPATH "GNU addr2line")
set(CMAKE_CXX_COMPILER_AR aarch64-linux-gnu-gcc-ar CACHE FILEPATH "GNU compiler archiver")
set(CMAKE_CXX_COMPILER_RANLIB aarch64-linux-gnu-gcc-ranlib CACHE FILEPATH "GNU compiler ranlib")
set(CMAKE_NM aarch64-linux-gnu-nm CACHE FILEPATH "GNU ranlib")
set(CMAKE_OBJCOPY aarch64-linux-gnu-objcopy CACHE FILEPATH "GNU ranlib")
set(CMAKE_OBJDUMP aarch64-linux-gnu-objdump CACHE FILEPATH "GNU objdump")
set(CMAKE_READELF aarch64-linux-gnu-readelf CACHE FILEPATH "GNU readelf")
set(CMAKE_STRIP aarch64-linux-gnu-strip CACHE FILEPATH "GNU strip")
