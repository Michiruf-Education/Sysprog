cmake_minimum_required(VERSION 3.6)
project(sysprog)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

# Make is using same directories as CMakeLists, so we do not need to care about this
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${sysprog_SOURCE_DIR}/quiz/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/quiz/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/quiz/bin)
#set_target_properties( targets...
#   PROPERTIES
#   ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#   LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
#)

add_custom_target(sysprog COMMAND make -C ${sysprog_SOURCE_DIR})
