cmake_minimum_required(VERSION 3.17)

project(projectname)
set(PROJECT_LIBRARY ${PROJECT_NAME}lib)
set(PROJECT_TESTS ${PROJECT_NAME}test)

file(GLOB SOURCES
    src/*.cpp
)
file(GLOB TEST_SOURCES tests/*.cpp)
list(REMOVE_ITEM SOURCES src/main.cpp)

# External Dependencies

# For Single File Headers:
#   include_directories(dependencies/...)
# For Larger Libraries:
#   add_subdirectory(dependencies/...)

# Library & Executables

add_library(${PROJECT_LIBRARY} ${SOURCES})
# For every external library:
#   target_link_libraries(${PROJECT_LIBRARY} ...)
target_include_directories(${PROJECT_LIBRARY} PUBLIC src)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBRARY})

# Testing

option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
    # Google Test Dependency
    #   add_subdirectory(dependencies/googletest)
    
    enable_testing()
    include(CTest)
    
    add_executable(${PROJECT_TESTS} ${TEST_SOURCES})
    target_link_libraries(${PROJECT_TESTS} gtest gtest_main)
    target_link_libraries(${PROJECT_TESTS} ${PROJECT_LIB})
    # include_directories(src)

    add_test(UnitTests ${PROJECT_TESTS})
endif()