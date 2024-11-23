cmake_minimum_required(VERSION 3.26)

project(bulk_prj)

############################################# BOOST #############################################

if(${CMAKE_VERSION} VERSION_LESS "3.30")
    message(STATUS "CMake version is ${CMAKE_VERSION}. Used CMP0167 OLD")    
else()
    message(STATUS "CMake version >= 3.30. Used CMP0167 NEW. Need to set BOOST_ROOT and Boost_DIR")
    cmake_policy(SET CMP0167 NEW)
    cmake_policy(SET CMP0144 NEW)
    set (BOOST_ROOT "C:/Program Files/boost/boost_1_85_0")
    set (Boost_DIR "${BOOST_ROOT}/bin/x64/lib/cmake/Boost-1.85.0")
endif()

set(Boost_DEBUG OFF)

find_package (Boost 1.75.0 COMPONENTS system REQUIRED) 

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found!")
endif()

include_directories (SYSTEM ${Boost_INCLUDE_DIR})


####################################################################################################


function(set_common_target_properties target)
    target_link_libraries(${target} PRIVATE
        ${Boost_LIBRARIES}
    )

    target_compile_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
    )
    set_target_properties(${target} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
    )    
endfunction()

add_subdirectory(server)