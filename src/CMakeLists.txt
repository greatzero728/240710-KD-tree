cmake_minimum_required(VERSION 3.8)
project(cs350-lib)

# Engine library
add_library(
    ${PROJECT_NAME}
    KdTree.hpp
    KdTree.cpp
    Shapes.hpp
    Shapes.cpp
    ShapeUtils.hpp
    ShapeUtils.cpp
    CS350Loader.hpp
    CS350Loader.cpp
    Geometry.hpp
    Geometry.cpp
    Logging.hpp
    Logging.cpp
    Window.hpp
    Window.cpp
    OpenGl.hpp
    OpenGl.cpp
    Utils.hpp
    Utils.cpp
    Shader.hpp
    Shader.cpp
    Primitive.hpp
    Primitive.cpp
    Camera.hpp
    Camera.cpp
    Math.hpp
    DebugRenderer.hpp
    DebugRenderer.cpp
    PRNG.h
    PRNG.cpp
    Stats.hpp
    Stats.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC .)

# GLM
find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)

# GLAD
find_package(glad CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC glad::glad)

# GLFW3
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)

# fmt
find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt)
