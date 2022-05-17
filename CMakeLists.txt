# You should have a path_to_toolchain.cmake file with
# a path to your cross-compilation tools with the following line:
# set(PATH_TO_TOOLCHAIN "real_path_to_your_toolchain")
# for example
# set(PATH_TO_TOOLCHAIN "D:/Tools/GCC")

include(${CMAKE_SOURCE_DIR}/../path_to_toolchain.cmake)
set(CMAKE_TOOLCHAIN_FILE raspberry.cmake)

#set(CMAKE_VERBOSE_MAKEFILE ON)

cmake_minimum_required(VERSION 3.17)

project(reaterra)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

file(GLOB MODBUS_FILES ${PROJECT_SOURCE_DIR}/libmodbus/src/*.c)
add_library(modbus OBJECT ${MODBUS_FILES})
include_directories("${PROJECT_SOURCE_DIR}/libmodbus/src")

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/reaterra/src/*.cpp)
add_executable(reaterra ${SRC_FILES})

target_link_libraries(reaterra PRIVATE
        Threads::Threads
        gio-2.0
        asound
        gobject-2.0
        glib-2.0
        mariadb
        cairo
        udev
        sqlite3
        modbus)
