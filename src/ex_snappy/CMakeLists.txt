#------------------------------------------------------------
# Configure external project
#------------------------------------------------------------

IF ( EMSCRIPTEN )
    # Use supplied emscripten version
    IF ( NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/emscripten/.libs/libsnappy.bc )
        MESSAGE ( STATUS "Unable to find libsnappy.bc" )
        MESSAGE ( STATUS "Make sure you ran the following build instructions:" )
        MESSAGE ( STATUS " * cd src/ex_snappy/emscripten" )
        MESSAGE ( STATUS " * sh autogen.sh" )
        MESSAGE ( STATUS " * emconfigure ./configure" )
        MESSAGE ( STATUS " * emmake make" )
        MESSAGE ( STATUS " * mv .libs/libsnappy.so.1.3.1 .libs/libsnappy.bc" )
        MESSAGE ( FATAL_ERROR "Aborting build" )
    ENDIF ( )

    ADD_LIBRARY( ex_snappy SHARED IMPORTED GLOBAL )
    SET_PROPERTY( TARGET ex_snappy PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/emscripten )
    SET_PROPERTY( TARGET ex_snappy PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/emscripten/.libs/libsnappy.bc )
ELSE ( )
    # Default unix find_package
    FIND_PACKAGE ( Snappy )

    ADD_LIBRARY( ex_snappy SHARED IMPORTED GLOBAL )
    SET_PROPERTY( TARGET ex_snappy PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${SNAPPY_INCLUDE_DIR} )
    SET_PROPERTY( TARGET ex_snappy PROPERTY IMPORTED_LOCATION ${SNAPPY_LIBRARIES} )
ENDIF ( )
