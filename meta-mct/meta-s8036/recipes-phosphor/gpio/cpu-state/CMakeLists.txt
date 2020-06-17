cmake_minimum_required(VERSION 3.6)
project(caterrHandler)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (
    CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -lstdc++fs \
    -Wall \
    -Wextra \
    -Wnon-virtual-dtor \
    -Wold-style-cast \
    -Wcast-align \
    -Wunused \
    -Woverloaded-virtual \
    -Wpedantic \
    -Wmisleading-indentation \
    -Wduplicated-cond \
    -Wduplicated-branches \
    -Wlogical-op \
    -Wnull-dereference \
    -Wuseless-cast \
    -Wdouble-promotion \
    -Wformat=2 \
    -Wno-sign-compare \
     -Wno-reorder \
    "
  )

set (LINK_LIBS -lsystemd stdc++fs sdbusplus peci)

add_executable (caterrHandler caterrHandler.cpp)
target_link_libraries(caterrHandler ${LINK_LIBS})

install(TARGETS caterrHandler
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)

