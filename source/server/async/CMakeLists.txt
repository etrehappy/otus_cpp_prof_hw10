cmake_minimum_required(VERSION 3.12)

project(async_lib_prj)

set(Parser    
    parser.h parser.cpp
)
set(CommandReader
    cmd_reader.h cmd_reader.cpp    
)

set(AsyncFiles
    async.h async.cpp    
    m_exceptions.h
    safe_queue.h safe_queue.cpp
    controller.h controller.cpp
)

if(WIN32)
    set (CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif ()

add_library(async SHARED ${AsyncFiles} ${Parser} ${CommandReader})

if(MSVC)
        source_group("Async" FILES ${AsyncFiles})
        source_group("Parser" FILES ${Parser})
        source_group("CommandReader" FILES ${CommandReader})
endif ()

install(TARGETS async
    RUNTIME DESTINATION ${ASYNC_LIB_BUILD_PATH}/bin           # .dll
    LIBRARY DESTINATION ${ASYNC_LIB_BUILD_PATH}/lib           # .so Unix
    ARCHIVE DESTINATION ${ASYNC_LIB_BUILD_PATH}/lib           # .lib, .a 
)

install(FILES async.h
    DESTINATION ${ASYNC_LIB_BUILD_PATH}/include
)

target_compile_options(async PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX >
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
        )

set_target_properties(async PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
)

