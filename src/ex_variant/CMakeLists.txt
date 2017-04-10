#------------------------------------------------------------
# Configure external project
#------------------------------------------------------------

ADD_LIBRARY ( ex_variant INTERFACE )
TARGET_INCLUDE_DIRECTORIES( ex_variant
    INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/upstream/include>
              $<INSTALL_INTERFACE:butterfly/eggs>
)

#------------------------------------------------------------
# Install
#------------------------------------------------------------

INSTALL (
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/upstream/include/eggs
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/butterfly
)

INSTALL (
    TARGETS ex_variant EXPORT ButterflyExVariantConfig
)

INSTALL ( EXPORT ButterflyExVariantConfig DESTINATION share/butterfly/cmake )
EXPORT ( TARGETS ex_variant FILE ButterflyExVariantConfig.cmake )
