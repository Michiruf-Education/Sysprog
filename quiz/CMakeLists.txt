cmake_minimum_required(VERSION 3.6)
project(sysprog)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

# Make is using same directories as CMakeLists, so we do not need to care about this
# TODO remove comment because untrue
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${sysprog_SOURCE_DIR}/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#set_target_properties( targets...
#   PROPERTIES
#   ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#   LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#   RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
#)

# Prepare output structure
#set(PROJECT_BINARY_DIR ${sysprog_SOURCE_DIR}/bin)
#set(sysprog_BINARY_DIR ${sysprog_SOURCE_DIR}/bin)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${sysprog_SOURCE_DIR}/bin)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")

# Add all files as sources (just for the IDE because building is extern)
# TODO MY_SRC could be useless
file(GLOB MY_SRC
        "*/*.h"
        "*/*.c"
)
set(SOURCE_FILES ${MY_SRC})

add_custom_target(server COMMAND make -C ${sysprog_SOURCE_DIR}
        #WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        #SOURCES ${SOURCE_FILES}
        CLION_EXE_DIR=${PROJECT_BINARY_DIR}/bin)
execute_process(COMMAND ${sysprog_SOURCE_DIR}/bin/server)
#set(EXECUTABLE_OUTPUT_PATH ${sysprog_SOURCE_DIR}/bin/server)


# Print all varaibles for debugging
if(true)
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endif()
