cmake_minimum_required(VERSION 3.20)

project(tide)
set(PROJECT_LIB tide_lib)
set(PROJECT_TESTS tide_tests)
set(PROJECT_TIMING tide_timing)

set(CMAKE_CXX_STANDARD 17)

if(BUILD_TYPE EQUAL 0)
    # Debug
    message("[BUILD TYPE] Debug")
    set(CMAKE_BUILD_TYPE "Debug")
    set(CMAKE_CXX_FLAGS "-std=gnu++17 -Wall -Werror -g")
    set(BUILD_TESTS "OFF")
elseif(BUILD_TYPE EQUAL 1)
    # Release
    message("[BUILD TYPE] Release (With Tests)")
    set(CMAKE_BUILD_TYPE "Release")
    set(CMAKE_CXX_FLAGS "-std=gnu++17 -Wall -O3")
    set(BUILD_TESTS "ON")
elseif(BUILD_TYPE EQUAL 2)
    # Release (No Tests)
    message("[BUILD TYPE] Release (No Tests)")
    set(CMAKE_BUILD_TYPE "Release")
    set(CMAKE_CXX_FLAGS "-std=gnu++17 -Wno-error=conversion -Werror -Wall -O3")
endif()

# Sources and Includes

file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/*.h
)
list(REMOVE_ITEM SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

include(CMakePrintHelpers)

# External Dependencies

# include_directories(dependencies/openal-soft/include)
# link_directories(dependencies/openal-soft)

add_subdirectory(dependencies/stb)
add_subdirectory(dependencies/freetype)
add_subdirectory(dependencies/glfw)
add_subdirectory(dependencies/glad)
add_subdirectory(dependencies/glm)
add_subdirectory(dependencies/fmt)

# Making Libraries and Executable

add_library(${PROJECT_LIB} ${SOURCES})
target_link_libraries(${PROJECT_LIB} stb)
target_link_libraries(${PROJECT_LIB} freetype)
target_link_libraries(${PROJECT_LIB} glfw)
target_link_libraries(${PROJECT_LIB} glad)
target_link_libraries(${PROJECT_LIB} glm)
target_link_libraries(${PROJECT_LIB} fmt)
target_include_directories(${PROJECT_LIB} PUBLIC src)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} ${PROJECT_LIB})

add_executable(${PROJECT_TIMING} tests/optimisation.cpp)
target_link_libraries(${PROJECT_TIMING} ${PROJECT_LIB})

add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> "../../build"
    COMMAND ${CMAKE_COMMAND} -E rm $<TARGET_FILE:${PROJECT_NAME}>
    COMMENT "Copying ${PROJECT_NAME} to build folder"
)

add_custom_command(
    TARGET ${PROJECT_TIMING}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_TIMING}> "../../build"
    COMMAND ${CMAKE_COMMAND} -E rm $<TARGET_FILE:${PROJECT_TIMING}>
    COMMENT "Copying ${PROJECT_TIMING} to build folder"
)