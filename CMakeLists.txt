cmake_minimum_required(VERSION 3.20)
project(elamite C)

add_compile_options(-std=c23)

find_package(PkgConfig REQUIRED)
pkg_check_modules(BACKTRACE REQUIRED libbacktrace)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

file(GLOB_RECURSE ELX_SOURCES "src/**.c")

add_executable(elx ${ELX_SOURCES})
target_include_directories(elx PRIVATE ${BACKTRACE_INCLUDE_DIRS})
target_link_libraries(elx PRIVATE ${BACKTRACE_LIBRARIES})
target_include_directories(elx PUBLIC "src/")
