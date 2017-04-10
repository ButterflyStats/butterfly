#------------------------------------------------------------
# Common
#------------------------------------------------------------

PROJECT ( butterfly_javascript VERSION 1.0 )

#------------------------------------------------------------
# Compile & Link
#------------------------------------------------------------

IF ( EMSCRIPTEN )
    ADD_EXECUTABLE( butterflyjs0 private/api0.cpp )
    TARGET_LINK_LIBRARIES( butterflyjs0 butterfly )
    SET_TARGET_PROPERTIES( butterflyjs0 PROPERTIES
        COMPILE_FLAGS "--bind"
        LINK_FLAGS "--bind --pre-js ${CMAKE_CURRENT_SOURCE_DIR}/pre.js"
    )

    ADD_EXECUTABLE( butterflyjs1 private/api1.cpp )
    TARGET_LINK_LIBRARIES( butterflyjs1 butterfly )
    SET_TARGET_PROPERTIES( butterflyjs1 PROPERTIES
        COMPILE_FLAGS "--bind"
        LINK_FLAGS "--bind --pre-js ${CMAKE_CURRENT_SOURCE_DIR}/pre.js"
    )
ENDIF ( EMSCRIPTEN )
