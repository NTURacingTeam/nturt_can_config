cmake_minimum_required(VERSION 3.8)
project(nturt_can_config)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)

# can database file
set(DBC_FILE nturt_can_config.dbc)
get_filename_component(DBC_FILE_BASENAME ${DBC_FILE} NAME_WE)

# generate c code from DBC_FILE and use physical values
execute_process(
        COMMAND ./generate_c_code.sh -p ${DBC_FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

include_directories(
        generated_code/${DBC_FILE_BASENAME}/include
)

# get all files under "generated_code/${DBC_FILE_BASENAME}/src" and set them to cpp sources
file(GLOB SOURCES generated_code/${DBC_FILE_BASENAME}/src/*.c)
set_source_files_properties(${SOURCES} PROPERTIES LANGUAGE CXX)

# libraries
# library: nturt_can_config
add_library(${PROJECT_NAME} SHARED
        ${SOURCES}
)

if(BUILD_TESTING)
        find_package(ament_lint_auto REQUIRED)
        # the following line skips the linter which checks for copyrights
        # comment the line when a copyright and license is added to all source files
        set(ament_cmake_copyright_FOUND TRUE)
        # the following line skips cpplint (only works in a git repo)
        # comment the line when this package is in a git repo and when
        # a copyright and license is added to all source files
        set(ament_cmake_cpplint_FOUND TRUE)
        ament_lint_auto_find_test_dependencies()
endif()

# export package dependencies
ament_export_include_directories(
        generated_code/${DBC_FILE_BASENAME}/include
)
ament_export_libraries(
        ${PROJECT_NAME}
)
ament_export_dependencies(
        ament_cmake
)

ament_package()
