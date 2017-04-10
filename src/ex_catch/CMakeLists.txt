#------------------------------------------------------------
# Configure external project
#------------------------------------------------------------

ADD_LIBRARY ( ex_catch INTERFACE )
TARGET_INCLUDE_DIRECTORIES( ex_catch
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/upstream/include
)
