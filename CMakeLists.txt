cmake_minimum_required(VERSION 3.15)
project(SkyrimDiskCache LANGUAGES CXX)

# the dll must not depend on a Visual C++ redistributable being installed
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

add_subdirectory(src)
