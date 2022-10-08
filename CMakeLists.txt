################# CONFIG ##################

#init Cmake
cmake_minimum_required(VERSION 3.24)
project(LushEngine LANGUAGES C CXX VERSION 1.0)

#set flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(UNIX)
    set(CMAKE_CXX_FLAGS "-std=c++20 -O3 -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Werror")
else()
    set(CMAKE_CXX_FLAGS "/std:c++20 /Ox /Wall /EHsc")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /DEBUG:FULL")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /WX")
endif()
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
set(CMAKE_THREADS_LIBS_INIT Threads::Threads)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set(DBUILD_SHARED_LIBS TRUE)

set(ASSIMP_BUILD_ASSIMP_VIEW OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(ASSIMP_BUILD_SAMPLES OFF)
set(ASSIMP_BUILD_TESTS OFF)

set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
set(ASSIMP_BUILD_COLLADA_IMPORTER ON)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)

#set binary/executable name
set(EXEC_NAME lush)

#fetch include paths
set(INC
    ${PROJECT_NAME}/
    ${PROJECT_NAME}/Entity/
    ${PROJECT_NAME}/Component/
    ${PROJECT_NAME}/System/
    ${PROJECT_NAME}/System/Render/
    ${PROJECT_NAME}/System/Control/
    ${PROJECT_NAME}/System/GUI/
    # ${PROJECT_NAME}/Communication/
    # ${PROJECT_NAME}/Audio/
    # ${PROJECT_NAME}/Core/
    # ${PROJECT_NAME}/Input/
    # ${PROJECT_NAME}/Loader/
    # ${PROJECT_NAME}/Render/
    # ${PROJECT_NAME}/Render/Objects/
    ${PROJECT_NAME}/IMGUI/
    ${PROJECT_NAME}/STB/
)

#fetch source files
set(SRC
    ${PROJECT_NAME}/Engine.cpp
    ${PROJECT_NAME}/Entity/EntityManager.cpp
    ${PROJECT_NAME}/Component/ComponentManager.cpp
    ${PROJECT_NAME}/Component/Component.cpp
    ${PROJECT_NAME}/System/SystemManager.cpp
    ${PROJECT_NAME}/System/Render/RenderSystem.cpp
    ${PROJECT_NAME}/System/Control/ControlSystem.cpp
    ${PROJECT_NAME}/System/GUI/GUISystem.cpp

    # ${PROJECT_NAME}/Communication/SafeQueue.cpp
    # ${PROJECT_NAME}/Communication/Packet.cpp
    # ${PROJECT_NAME}/Communication/Message.cpp
    # ${PROJECT_NAME}/Communication/Node.cpp
    # ${PROJECT_NAME}/Communication/MessageBus.cpp
    # ${PROJECT_NAME}/Audio/Audio.cpp
    # ${PROJECT_NAME}/Core/Core.cpp
    # ${PROJECT_NAME}/Core/Scene.cpp
    # ${PROJECT_NAME}/Core/GameObject.cpp
    # ${PROJECT_NAME}/Input/Input.cpp
    # ${PROJECT_NAME}/Loader/Loader.cpp
    # ${PROJECT_NAME}/Render/Render.cpp
    # ${PROJECT_NAME}/Render/Camera.cpp
    # ${PROJECT_NAME}/Render/Shader.cpp
    # ${PROJECT_NAME}/Render/RenderObject.cpp
    # ${PROJECT_NAME}/Render/Mesh.cpp
    # ${PROJECT_NAME}/Render/Bone.cpp
    # ${PROJECT_NAME}/Render/Model.cpp
    # ${PROJECT_NAME}/Render/Objects/AnimatedModel.cpp
    # ${PROJECT_NAME}/Render/Objects/StaticModel.cpp
    ${PROJECT_NAME}/IMGUI/imgui_draw.cpp
    ${PROJECT_NAME}/IMGUI/imgui_tables.cpp
    ${PROJECT_NAME}/IMGUI/imgui_widgets.cpp
    ${PROJECT_NAME}/IMGUI/imgui.cpp
    ${PROJECT_NAME}/IMGUI/imgui_impl_glfw.cpp
    ${PROJECT_NAME}/IMGUI/imgui_impl_opengl3.cpp
    ${PROJECT_NAME}/STB/stb.cpp
)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
# LATER find_package(ASSIMP REQUIRED)

#set include paths
include_directories(${INC}
    ${OpenGL_INCLUDE_DIRS}
    ${GLFW3_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
)

#set source files
add_executable(${EXEC_NAME}
    ${SRC}
    ${PROJECT_NAME}/main.cpp
)

target_link_libraries(${EXEC_NAME} PRIVATE ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${EXEC_NAME} PRIVATE ${OPENGL_LIBRARIES})
target_link_libraries(${EXEC_NAME} PRIVATE glfw)
target_link_libraries(${EXEC_NAME} PRIVATE ${GLEW_LIBRARY})
# LATER target_link_libraries(${EXEC_NAME} PRIVATE assimp)
set_target_properties(${EXEC_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set_target_properties(${EXEC_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

#install target
install(TARGETS ${EXEC_NAME})

################## POST BUILD ##################
