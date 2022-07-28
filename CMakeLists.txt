#init Cmake
cmake_minimum_required(VERSION 3.16)

project(LushEngine LANGUAGES CXX VERSION 1.0)

#set standard flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-std=c++17 -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")

#set thread flags
set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_HAVE_THREADS_LIBRARY true)
set(CMAKE_USE_WIN32F_THREADS_INIT OFF)
set(CMAKE_USE_PTHREADS_INIT true)
set(ASSIMP_BUILD_ASSIMP_VIEW OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_SAMPLES OFF)
set(ASSIMP_BUILD_TESTS OFF)

set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_COLLADA_IMPORTER ON)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)

if(UNIX)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")
else()
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /Ox")
endif()

#set binary/executable name
set(EXEC_NAME lush)

#fetch include paths
set(INC
    ${PROJECT_NAME}/
    ${PROJECT_NAME}/Communication/
    ${PROJECT_NAME}/Audio/
    ${PROJECT_NAME}/Core/
    ${PROJECT_NAME}/Input/
    ${PROJECT_NAME}/Loader/
    ${PROJECT_NAME}/Render/
)

#fetch source files
set(SRC
    ${PROJECT_NAME}/Engine.cpp
    ${PROJECT_NAME}/Communication/SafeQueue.cpp
    ${PROJECT_NAME}/Communication/Packet.cpp
    ${PROJECT_NAME}/Communication/Message.cpp
    ${PROJECT_NAME}/Communication/Node.cpp
    ${PROJECT_NAME}/Communication/MessageBus.cpp
    ${PROJECT_NAME}/Audio/Audio.cpp
    ${PROJECT_NAME}/Core/Core.cpp
    ${PROJECT_NAME}/Core/GameObject.cpp
    ${PROJECT_NAME}/Input/Input.cpp
    ${PROJECT_NAME}/Loader/Loader.cpp
    ${PROJECT_NAME}/Render/Render.cpp
    ${PROJECT_NAME}/Render/Camera.cpp
    ${PROJECT_NAME}/Render/Shader.cpp
    ${PROJECT_NAME}/Render/RenderObject.cpp
)

find_package(OpenGL REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(ASSIMP REQUIRED)

#set source files
add_executable(${EXEC_NAME}
    ${SRC}
    ${PROJECT_NAME}/main.cpp
)

target_compile_options(${EXEC_NAME} PRIVATE  -Wall -Wextra -Werror -g)

#set include paths
include_directories(${INC}
    ${OpenGL_INCLUDE_DIRS}
    ${GLFW3_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
)

target_link_libraries(${EXEC_NAME}
    ${CMAKE_THREAD_LIBS_INIT}
    ${CMAKE_THREAD_LIBS_REQUIRED}
    ${OPENGL_LIBRARIES}
    ${GLFW3_LIBRARIES}
    glfw
    ${GLEW_LIBRARIES}
    glew
    ${ASSIMP_LIBRARIES}
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
