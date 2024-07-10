cmake_minimum_required(VERSION 3.8)
project(cs350-test)

############################
# Testing
add_executable(${PROJECT_NAME}
        Common.hpp Common.cpp
        A-Common.cpp
        A5-KdTree.cpp
        )
target_link_libraries(${PROJECT_NAME} PUBLIC cs350-lib)

# lodepng
find_package(lodepng CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE lodepng)

# fmt
find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

# GTest
enable_testing()
find_package(GTest CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})



