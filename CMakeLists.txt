#------------------------------------------------------------
# Standard Stuff
#------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED ( VERSION 3.2 FATAL_ERROR )
PROJECT ( butterfly VERSION 0.1 )

SET ( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake )

MESSAGE ( STATUS "Source DIR is ${PROJECT_SOURCE_DIR}" )
MESSAGE ( STATUS "Binary DIR is ${PROJECT_BINARY_DIR}" )
MESSAGE ( STATUS "Build type is ${CMAKE_BUILD_TYPE}" )
MESSAGE ( STATUS "Libs extension is '${SUFFIXLIB}'. Bins extension is '${SUFFIXBIN}'." )
MESSAGE ( STATUS "Installation prefix directory is " ${CMAKE_INSTALL_PREFIX} )
MESSAGE ( STATUS "Host system is " ${CMAKE_HOST_SYSTEM} " with processor " ${CMAKE_HOST_SYSTEM_PROCESSOR} )
MESSAGE ( STATUS "Target system is " ${CMAKE_SYSTEM} " with processor " ${CMAKE_SYSTEM_PROCESSOR} )
MESSAGE ( STATUS "Compiler identified as " ${CMAKE_CXX_COMPILER_ID} )

#------------------------------------------------------------
# Compiler Flags
#------------------------------------------------------------

SET( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++14 -Wall -O3 -g" )
SET( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++14 -Wall -O3 -march=native -fvisibility=hidden" )

# Default flags = release flags
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE}" )

INCLUDE ( Emscripten )
INCLUDE ( Util )

#------------------------------------------------------------
# Build options
#------------------------------------------------------------

OPTION ( WITH_PYTHON_BINDINGS "Build python bindings" 0 )
OPTION ( WITH_JAVASCRIPT_BINDINGS "Build javascript bindings, requires emscripten" 0 )
OPTION ( WITH_EXAMPLES "Build examples" 0 )
OPTION ( WITH_TOOLS "Build tools, only useful for development" 0 )

#------------------------------------------------------------
# Projects
#------------------------------------------------------------

# Internal libraries
ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/src/butterfly_proto )
ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/src/butterfly_trie )

# External libraries
ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/src/ex_catch )
ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/src/ex_snappy )
ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/src/ex_protobuf )
ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/src/ex_variant )

# Project
ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/src/butterfly )

# Bindings
IF ( ${WITH_PYTHON_BINDINGS} )
    ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/src/bindings_python )
ENDIF ( ${WITH_PYTHON_BINDINGS} )

IF ( ${WITH_JAVASCRIPT_BINDINGS} )
    ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/src/bindings_javascript )
ENDIF ( ${WITH_JAVASCRIPT_BINDINGS} )

#------------------------------------------------------------
# Examples
#------------------------------------------------------------

IF ( ${WITH_EXAMPLES} )
    SET ( BF_EXAMPLES 01-basic 02-spew-types 03-props 04-deathmap 05-seeking 06-events 07-combatlog )

    FOREACH ( EX ${BF_EXAMPLES} )
        ADD_EXECUTABLE ( ${EX} ${CMAKE_SOURCE_DIR}/examples/cpp/${EX}.cpp )
        TARGET_LINK_LIBRARIES ( ${EX} butterfly )
        SET_PROPERTY(TARGET ${EX} PROPERTY CXX_STANDARD 14)

        INSTALL(
            TARGETS ${EX}
            RUNTIME DESTINATION share/butterfly/bin
        )
    ENDFOREACH()
ENDIF ( ${WITH_EXAMPLES} )

#------------------------------------------------------------
# With tools
#------------------------------------------------------------

IF ( ${WITH_TOOLS} )
    ADD_SUBDIRECTORY( ${CMAKE_SOURCE_DIR}/src/tools )
ENDIF ( ${WITH_TOOLS} )
