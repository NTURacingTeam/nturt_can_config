cmake_minimum_required(VERSION 3.16)
project(nturt_can_config)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

option(FOR_ROS2 "Building for use in ros2" ON)

if(FOR_ROS2)
        find_package(ament_cmake REQUIRED)
endif()

# can database file
set(DBC_FILE nturt_can_config.dbc)
get_filename_component(DBC_FILE_BASENAME ${DBC_FILE} NAME_WE)

# generate c code from DBC_FILE and use physical values
execute_process(
        COMMAND ./generate_c_code.sh -p ${DBC_FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# get all files under "generated_code/${DBC_FILE_BASENAME}/src"
file(GLOB SOURCES generated_code/${DBC_FILE_BASENAME}/src/*.c)

if(FOR_ROS2)
        # set source files to cpp
        set_source_files_properties(${SOURCES} PROPERTIES LANGUAGE CXX)
endif()

# libraries
# library: nturt_can_config
add_library(${PROJECT_NAME}
        ${SOURCES}
)
target_include_directories(${PROJECT_NAME} PUBLIC
        generated_code/${DBC_FILE_BASENAME}/include
)

if(FOR_ROS2)
        # make the library position independent for shared library to link to
        set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

        # add extra utilitise for registering callbacks to ros2 nodes
        target_include_directories(${PROJECT_NAME} PUBLIC
                include
        )
        target_sources(${PROJECT_NAME} PRIVATE
                src/battery_utils.cpp
                src/can_callback_register.cpp
                src/can_timeout_monitor.cpp
        )

        # install headers
        install(DIRECTORY include/${PROJECT_NAME}
                DESTINATION include
        )
        install(DIRECTORY generated_code/${DBC_FILE_BASENAME}/include/
                DESTINATION include
        )

        # install libraries
        install(TARGETS ${PROJECT_NAME}
                RUNTIME DESTINATION bin
                ARCHIVE DESTINATION lib
                LIBRARY DESTINATION lib
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
                include
        )
        ament_export_libraries(
                ${PROJECT_NAME}
        )
        ament_export_dependencies(
                ament_cmake
        )

        ament_package()
endif()
