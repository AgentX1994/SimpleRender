cmake_minimum_required(VERSION 3.13.0)

project(OpenGlTest
    LANGUAGES CXX
)

find_package(PkgConfig REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)
find_package(spdlog REQUIRED)

pkg_check_modules(LibEpoxy REQUIRED IMPORTED_TARGET epoxy)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Werror -Wpedantic)

add_executable(OpenGlTest
    src/main.cpp
    src/debug_callback.cpp
    src/mesh.cpp
)

target_link_libraries(OpenGlTest
    PkgConfig::LibEpoxy
    glfw
    glm
    spdlog::spdlog
)
