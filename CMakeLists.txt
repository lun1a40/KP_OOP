cmake_minimum_required(VERSION 3.16)
project(ToyFactoryAssets CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Все .cpp из src/ собираются автоматически (CONFIGURE_DEPENDS — пересканирует при изменениях).
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(ToyFactoryAssets ${SOURCES})

# src — для своих заголовков, third_party — для httplib.h и nlohmann/json.hpp
target_include_directories(ToyFactoryAssets PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/third_party")

# cpp-httplib требует потоки
find_package(Threads REQUIRED)
target_link_libraries(ToyFactoryAssets PRIVATE Threads::Threads)

if(MSVC)
    # /utf-8 — критично для кириллических литералов под MSVC
    target_compile_options(ToyFactoryAssets PRIVATE /utf-8 /W3)
else()
    target_compile_options(ToyFactoryAssets PRIVATE -Wall -Wextra)
endif()
