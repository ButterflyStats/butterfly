#------------------------------------------------------------
# Common
#------------------------------------------------------------

PROJECT ( butterfly_proto VERSION 1.0 )

#------------------------------------------------------------
# Generate protobuf sources
#------------------------------------------------------------

SET ( PROTO_FILES demo dota_commonmessages dota_clientmessages dota_modifiers dota_gcmessages_common
    dota_shared_enums dota_usermessages gameevents gcsdk_gcmessages networkbasetypes netmessages network_connection
    usermessages steammessages )

FOREACH ( PROTO ${PROTO_FILES} )
    # Generate protobuf
    EXECUTE_PROCESS ( COMMAND protoc
         ${CMAKE_SOURCE_DIR}/proto/${PROTO}.proto
         --cpp_out=${CMAKE_SOURCE_DIR}/src/butterfly_proto/
         --proto_path=${CMAKE_SOURCE_DIR}/proto
    )

    # Move header
    MOVE (
        ${CMAKE_SOURCE_DIR}/src/butterfly_proto/${PROTO}.pb.h
        ${CMAKE_SOURCE_DIR}/src/butterfly_proto/public/butterfly/proto/${PROTO}.pb.h
    )

    # Move src
    MOVE (
        ${CMAKE_SOURCE_DIR}/src/butterfly_proto/${PROTO}.pb.cc
        ${CMAKE_SOURCE_DIR}/src/butterfly_proto/private/${PROTO}.pb.cc
    )

    LIST( APPEND PROTO_SOURCES ${CMAKE_SOURCE_DIR}/src/butterfly_proto/private/${PROTO}.pb.cc )
    LIST( APPEND PROTO_HEADERS ${CMAKE_SOURCE_DIR}/src/butterfly_proto/public/butterfly/proto/${PROTO}.pb.h )
ENDFOREACH()

#------------------------------------------------------------
# Compile
#------------------------------------------------------------

ADD_LIBRARY ( butterfly_proto STATIC ${PROTO_SOURCES} )

TARGET_INCLUDE_DIRECTORIES( butterfly_proto
    PRIVATE ${CMAKE_SOURCE_DIR}/src/butterfly_proto/public/butterfly/proto
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/public>
           $<INSTALL_INTERFACE:butterfly/proto>
)

TARGET_LINK_LIBRARIES( butterfly_proto PUBLIC ex_protobuf )

#------------------------------------------------------------
# Install
#------------------------------------------------------------

INSTALL (
    TARGETS butterfly_proto EXPORT ButterflyProtoConfig
    ARCHIVE DESTINATION lib
)

INSTALL ( DIRECTORY ${CMAKE_SOURCE_DIR}/proto DESTINATION share/butterfly )
INSTALL ( DIRECTORY ${CMAKE_SOURCE_DIR}/src/butterfly_proto/public/butterfly DESTINATION include )
INSTALL ( EXPORT ButterflyProtoConfig DESTINATION share/butterfly/cmake )

EXPORT ( TARGETS butterfly_proto FILE ButterflyProtoConfig.cmake )
