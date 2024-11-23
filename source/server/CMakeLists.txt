project(bulk_server_prj)

include_directories(${CMAKE_BINARY_DIR})

set(EXE_NAME bulk_server)
set(SRC_PATH ${CMAKE_SOURCE_DIR}/server)
set(ASYNC_LIB_BUILD_PATH ${CMAKE_BINARY_DIR}/server/async/)

set(SERVER_FILES
    server.cpp 
)

add_subdirectory(async)

add_executable(${EXE_NAME} 
    ${SERVER_FILES} 
)

target_include_directories(${EXE_NAME} PRIVATE ${ASYNC_LIB_BUILD_PATH}/include)
target_link_directories(${EXE_NAME} PRIVATE ${ASYNC_LIB_BUILD_PATH}/lib) 
target_link_libraries(${EXE_NAME} PRIVATE async)

if(MSVC)
    set_property(DIRECTORY ${SRC_PATH} PROPERTY VS_STARTUP_PROJECT ${EXE_NAME})	
    source_group("Server" FILES ${SERVER_FILES})
endif ()

set_common_target_properties(${EXE_NAME})

add_custom_command(TARGET ${EXE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:async>
        $<TARGET_FILE_DIR:${EXE_NAME}>
        COMMAND ${CMAKE_COMMAND} -E echo "Copying $<TARGET_FILE:async> to ${CMAKE_BINARY_DIR}/server"
)
