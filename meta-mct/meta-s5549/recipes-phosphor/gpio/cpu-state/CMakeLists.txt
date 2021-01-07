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

option (
    CATERR_ENABLE_POWER_FILTER
    "Enable CATERR event power filter."
    OFF
  )
option (
    PROCHOT_ENABLE_POWER_FILTER
    "Enable PROCHOT event power filter."
    OFF
  )
option (
    THERMTRIP_ENABLE_POWER_FILTER
    "Enable THERMTRIP event power filter."
    OFF
  )

set (CATERR_LINK_LIBS -lsystemd stdc++fs sdbusplus peci)

add_executable (caterrHandler caterrHandler.cpp)
target_link_libraries(caterrHandler ${CATERR_LINK_LIBS})

install(TARGETS caterrHandler
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)

target_compile_definitions (
    caterrHandler PRIVATE $<$<BOOL:${CATERR_ENABLE_POWER_FILTER}>: -DCATERR_ENABLE_POWER_FILTER>
)

set (PROCHOT_THERMTRIP_LINK_LIBS -lsystemd stdc++fs sdbusplus -lgpiod -lobmcmisc)

add_executable (prochotThermtripHandler prochotThermtripHandler.cpp)
target_link_libraries(prochotThermtripHandler ${PROCHOT_THERMTRIP_LINK_LIBS})

install(TARGETS prochotThermtripHandler
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)

target_compile_definitions (
    prochotThermtripHandler PRIVATE $<$<BOOL:${PROCHOT_ENABLE_POWER_FILTER}>: -DPROCHOT_ENABLE_POWER_FILTER>
    $<$<BOOL:${THERMTRIP_ENABLE_POWER_FILTER}>: -DTHERMTRIP_ENABLE_POWER_FILTER>
)
