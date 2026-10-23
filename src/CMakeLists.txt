add_library(diskCacheEnabler MODULE dcache.cpp)
target_include_directories(diskCacheEnabler PRIVATE
  ${CMAKE_SOURCE_DIR}/deps/include/
)

target_link_directories(diskCacheEnabler PRIVATE
  ${CMAKE_SOURCE_DIR}/deps/libs/detours
)

target_link_libraries(diskCacheEnabler PRIVATE detours.lib)
