#------------------------------------------------------------
# Common
#------------------------------------------------------------

PROJECT ( butterfly_trie VERSION 1.0 )

#------------------------------------------------------------
# Compile as static & shared library
#------------------------------------------------------------

SET ( TRIE_SRC ${CMAKE_SOURCE_DIR}/src/butterfly_trie/private )
SET ( TRIE_SOURCES
    ${TRIE_SRC}/fmemopen.c
    ${TRIE_SRC}/ssv.cpp
    ${TRIE_SRC}/tx.cpp
)

ADD_LIBRARY ( butterfly_trie STATIC ${TRIE_SOURCES} )
TARGET_INCLUDE_DIRECTORIES( butterfly_trie
    PRIVATE ${CMAKE_SOURCE_DIR}/src/butterfly_trie/public/butterfly/trie
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/public>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/public/butterfly>
        $<INSTALL_INTERFACE:butterfly/trie>
)

#------------------------------------------------------------
# Install
#------------------------------------------------------------

INSTALL (
    TARGETS butterfly_trie EXPORT ButterflyTrieConfig
    ARCHIVE DESTINATION lib
)

INSTALL ( DIRECTORY ${CMAKE_SOURCE_DIR}/src/butterfly_trie/public/butterfly DESTINATION ${CMAKE_INSTALL_PREFIX}/include )
INSTALL ( EXPORT ButterflyTrieConfig DESTINATION share/butterfly/cmake)

EXPORT ( TARGETS butterfly_trie FILE ButterflyTrieConfig.cmake)
