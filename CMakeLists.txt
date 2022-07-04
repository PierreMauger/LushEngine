#init Cmake
cmake_minimum_required(VERSION 3.16)

project(Lush LANGUAGES CXX VERSION 1.0)

#set standard flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -Werror -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS} "-g")

if(UNIX)
    set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS} "-O3")
else()
    set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS} "/Ox")
endif()

#set binary/executable name
set(EXEC_NAME lush)

#fetch include paths
set(INC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

#fetch source files
set(SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

#set include paths
include_directories(${INC})

#set source files
add_executable(${EXEC_NAME}
    ${SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

if(WIN32)
    add_custom_command(
        TARGET ${EXEC_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/${EXEC_NAME}.exe ${CMAKE_CURRENT_SOURCE_DIR}
    )
else()
    add_custom_command(
        TARGET ${EXEC_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/${EXEC_NAME} ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()
