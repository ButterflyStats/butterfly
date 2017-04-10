## --
## -* Description:   Additional console colors
## --
IF ( NOT WIN32 )
  STRING( ASCII 27 Esc )
  SET( ColorReset       "${Esc}[m" )
  SET( ColorRed         "${Esc}[31m" )
  SET( ColorGreen       "${Esc}[32m" )
  SET( ColorYellow      "${Esc}[33m" )
  SET( ColorBlue        "${Esc}[34m" )
  SET( ColorMagenta     "${Esc}[35m" )
  SET( ColorCyan        "${Esc}[36m" )
  SET( ColorWhite       "${Esc}[37m" )
  SET( ColorBoldRed     "${Esc}[1;31m" )
  SET( ColorBoldGreen   "${Esc}[1;32m" )
  SET( ColorBoldYellow  "${Esc}[1;33m" )
  SET( ColorBoldBlue    "${Esc}[1;34m" )
  SET( ColorBoldMagenta "${Esc}[1;35m" )
  SET( ColorBoldCyan    "${Esc}[1;36m" )
  SET( ColorBoldWhite   "${Esc}[1;37m" )
ENDIF()

## --
## -* Description:   Write message as given color
## -* Usage:         messagec ( ${color} ${message} )
## --
FUNCTION( MESSAGEC )
    LIST ( GET ARGV 0 COLOR )
    LIST ( REMOVE_AT ARGV 0 )
    MESSAGE( STATUS "${Color${COLOR}}${ARGV}${ColorReset}")
ENDFUNCTION()


## --
## -* Description:   Move a file using cmake (platform independent)
## -* Usage:         move ( ${target} ${destination} )
## --
FUNCTION( MOVE )
    LIST ( GET ARGV 0 TARGET )
    LIST ( GET ARGV 1 DESTINATION )

    # Copy to target and remove original
    EXECUTE_PROCESS ( COMMAND ${CMAKE_COMMAND} -E copy ${TARGET} ${DESTINATION} )
    EXECUTE_PROCESS ( COMMAND ${CMAKE_COMMAND} -E remove ${TARGET} )
ENDFUNCTION()
