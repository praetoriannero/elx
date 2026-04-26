cmake_minimum_required(VERSION 3.20)
project(elamite C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)

add_compile_options(-std=c23)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

file(GLOB_RECURSE ELX_SOURCES "src/**.c")

add_executable(elx ${ELX_SOURCES})
target_include_directories(elx PUBLIC ${GLIB_INCLUDE_DIRS})
target_include_directories(elx PUBLIC "src/")
target_link_libraries(elx PUBLIC ${GLIB_LIBRARIES})
