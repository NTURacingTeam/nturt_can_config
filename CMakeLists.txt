cmake_minimum_required(VERSION 3.8)
project(nturt_can_config)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_STANDARD 99)

execute_process(
        COMMAND ./generate_c_code.sh nturt.dbc
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_QUIET
)

include_directories(
        generated_code/nturt/include
)

# libraries
# library: nturt_can_config
file(GLOB SOURCES "generated_code/nturt/src/*.c")
add_library(nturt_can_config SHARED
        ${SOURCES}
)
