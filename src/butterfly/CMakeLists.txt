#------------------------------------------------------------
# Common
#------------------------------------------------------------

PROJECT ( butterfly VERSION 1.0 )

#------------------------------------------------------------
# Compile & Link
#------------------------------------------------------------

SET ( BUTTERFLY_SRC ${CMAKE_SOURCE_DIR}/src/butterfly/private )
SET ( BUTTERFLY_SOURCES
    ${BUTTERFLY_SRC}/alloc.cpp
    ${BUTTERFLY_SRC}/combatlog.cpp
    ${BUTTERFLY_SRC}/dem.cpp
    ${BUTTERFLY_SRC}/demfile.cpp
    ${BUTTERFLY_SRC}/entity.cpp
    ${BUTTERFLY_SRC}/flattened_serializer.cpp
    ${BUTTERFLY_SRC}/parser.cpp
    ${BUTTERFLY_SRC}/particle.cpp
    ${BUTTERFLY_SRC}/property_decoder.cpp
    ${BUTTERFLY_SRC}/resources.cpp
    ${BUTTERFLY_SRC}/stringtable.cpp
    ${BUTTERFLY_SRC}/util_assert.cpp
    ${BUTTERFLY_SRC}/util_vpk.cpp
)

SET_SOURCE_FILES_PROPERTIES ( ${BUTTERFLY_SRC}/resources.cpp PROPERTIES COMPILE_FLAGS "-O1" )
ADD_LIBRARY ( butterfly STATIC ${BUTTERFLY_SOURCES} )

TARGET_INCLUDE_DIRECTORIES( butterfly
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/public>
           $<INSTALL_INTERFACE:butterfly>
)

TARGET_LINK_LIBRARIES( butterfly
    butterfly_proto butterfly_trie
    ex_snappy ex_protobuf ex_variant
)

SET_TARGET_PROPERTIES( butterfly PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

#------------------------------------------------------------
# Tests (optional)
#------------------------------------------------------------

IF ( 0 )
    ADD_EXECUTABLE ( butterfly_test
        ${CMAKE_CURRENT_SOURCE_DIR}/test/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/butterfly/util_assert.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/butterfly/util_chash.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/butterfly/util_delegate.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/butterfly/util_dict.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/butterfly/util_noncopyable.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/butterfly/util_platform.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/butterfly/util_ringbuffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/butterfly/util_string.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/butterfly/util_varint.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/butterfly/util_ztime.cpp
    )

    TARGET_LINK_LIBRARIES ( butterfly_test butterfly Catch2WithMain )

    INSTALL(
        TARGETS butterfly_test
        RUNTIME DESTINATION share/butterfly/bin
    )

    SET_TARGET_PROPERTIES( butterfly_test PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )
ENDIF ( 0 )

#------------------------------------------------------------
# Install
#------------------------------------------------------------

INSTALL (
    TARGETS butterfly EXPORT ButterflyConfig
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

INSTALL ( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/public/butterfly DESTINATION include )
INSTALL ( EXPORT ButterflyConfig DESTINATION share/butterfly/cmake )
EXPORT ( TARGETS butterfly FILE ButterflyConfig.cmake )
