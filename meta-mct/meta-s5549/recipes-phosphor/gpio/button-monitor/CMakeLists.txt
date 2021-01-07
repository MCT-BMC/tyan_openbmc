cmake_minimum_required (VERSION 3.5 FATAL_ERROR)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)


project(power-button-monitor)

add_executable(power-button-monitor power-button-monitor.cpp)
target_link_libraries (power-button-monitor -lsystemd)

project(reset-button-monitor)

add_executable(reset-button-monitor reset-button-monitor.cpp)
target_link_libraries (reset-button-monitor -lsystemd)


# Strip binary for release builds
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
        string (REGEX REPLACE "ar$" "gcc-ar" CMAKE_AR ${CMAKE_AR})
        string (
            REGEX
            REPLACE "ranlib$" "gcc-ranlib" CMAKE_RANLIB ${CMAKE_RANLIB}
        )
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto -fno-fat-lto-objects")

        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
        -fvisibility=hidden \
        -fvisibility-inlines-hidden \
        -Wl,--exclude-libs,ALL")
    endif (NOT CMAKE_BUILD_TYPE MATCHES Debug)
endif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

