#------------------------------------------------------------
# Common
#------------------------------------------------------------

PROJECT ( reslookup VERSION 1.0 )

#------------------------------------------------------------
# Compile & Link
#------------------------------------------------------------

ADD_EXECUTABLE ( reslookup ${CMAKE_CURRENT_SOURCE_DIR}/gen.cpp )

TARGET_INCLUDE_DIRECTORIES( reslookup
    PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/butterfly>
)

TARGET_LINK_LIBRARIES( reslookup PUBLIC
    butterfly
    butterfly_trie
)

#------------------------------------------------------------
# Install
#------------------------------------------------------------

INSTALL ( TARGETS reslookup RUNTIME DESTINATION bin )
