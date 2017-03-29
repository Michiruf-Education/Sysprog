########################################################################
###                               HOW TO
###---------------------------------------------------------------------
###
### Create 2 run configurations:
###
### 1.  "server"
###     Target:     sysprog
###     Executable: server (manually chosen from dialog in bin
###                 directory)
###     Args:       TODO
###
### 2.  "client"
###     Target:     sysprog
###     Executable: loader (manually chosen from dialog in bin
###                 directory)
###     Args:       TODO
###
### You may need to build using make first (for the binaries to exist)
###
########################################################################

cmake_minimum_required(VERSION 3.6)
project(sysprog)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

# Add all files as sources (just for the IDE because building is extern)
file(GLOB SOURCE_FILES
        "*/*.h"
        "*/*.c"
)

# Add fake executable to integrate files in IDE project
add_executable(sysprog_internal EXCLUDE_FROM_ALL ${SOURCE_FILES})

# Create custom target for make building
# See comment above on how to use
add_custom_target(sysprog COMMAND make -C ${sysprog_SOURCE_DIR})
# TODO remove additional fields (we do not need)
        #WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        #SOURCES ${SOURCE_FILES}
        #CLION_EXE_DIR=${PROJECT_BINARY_DIR}/bin)


# Print all varaibles for debugging
if(false)
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endif()
