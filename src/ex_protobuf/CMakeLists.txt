#------------------------------------------------------------
# Configure external project
#------------------------------------------------------------

IF ( EMSCRIPTEN )
    # Use supplied emscripten version
    IF ( NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/emscripten/2.6.1/src/.libs/libprotobuf.bc )
        MESSAGE ( STATUS "Unable to find libprotobuf.bc" )
        MESSAGE ( STATUS "Make sure you ran the following build instructions:" )
        MESSAGE ( STATUS " * cd src/ex_protobuf/emscripten/2.6.1" )
        MESSAGE ( STATUS " * git clone git@github.com:invokr/protobuf-emscripten.git ." )
        MESSAGE ( STATUS " * sh autogen.sh" )
        MESSAGE ( STATUS " * emconfigure ./configure" )
        MESSAGE ( STATUS " * emmake make" )
        MESSAGE ( FATAL_ERROR "Aborting build" )
    ENDIF ( )

    ADD_LIBRARY( ex_protobuf SHARED IMPORTED GLOBAL )
    SET_PROPERTY( TARGET ex_protobuf PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/emscripten/2.6.1/src )
    SET_PROPERTY( TARGET ex_protobuf PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/emscripten/2.6.1/src/.libs/libprotobuf.bc )
ELSE ( )
    # Default unix find_package
    FIND_PACKAGE ( Protobuf REQUIRED )

    ADD_LIBRARY( ex_protobuf SHARED IMPORTED GLOBAL )
    SET_PROPERTY( TARGET ex_protobuf PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${PROTOBUF_INCLUDE_DIR} )
    SET_PROPERTY( TARGET ex_protobuf PROPERTY IMPORTED_LOCATION ${PROTOBUF_LIBRARY} )
ENDIF ( )
