#------------------------------------------------------------
# Common
#------------------------------------------------------------

PROJECT ( butterfly_python VERSION 1.0 )
ADD_SUBDIRECTORY( pybind11 )

#------------------------------------------------------------
# Compile & Link
#------------------------------------------------------------

PYBIND11_ADD_MODULE( butterpy MODULE ${CMAKE_CURRENT_SOURCE_DIR}/private/bindings.cpp )
TARGET_LINK_LIBRARIES( butterpy PRIVATE butterfly )
