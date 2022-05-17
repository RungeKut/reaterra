
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)

SET(CMAKE_MAKE_PROGRAM "${PATH_TO_TOOLCHAIN}/bin/make.exe" FORCE)

set(CMAKE_C_COMPILER "${PATH_TO_TOOLCHAIN}/bin/arm-linux-gnueabihf-gcc.exe")
set(CMAKE_CXX_COMPILER "${PATH_TO_TOOLCHAIN}/bin/arm-linux-gnueabihf-g++.exe")

SET(COMMON_FLAGS "-O0 -g3 -Wall -pedantic-errors -fmessage-length=0")
SET(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=c++1y")
