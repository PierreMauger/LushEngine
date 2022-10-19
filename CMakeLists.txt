################## CONFIG ##################

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
set(BUILD_SHARED_LIBS TRUE)

# set(ASSIMP_BUILD_ASSIMP_VIEW OFF)
# set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
# set(ASSIMP_BUILD_SAMPLES OFF)
# set(ASSIMP_BUILD_TESTS OFF)

# set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
# set(ASSIMP_BUILD_COLLADA_IMPORTER ON)
# set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)

################## BUILD ECS ##################

set(ECS ECS)

set(ECS_ROOT ${PROJECT_NAME}/ECS/)

set(ECS_INC
    ${ECS_ROOT}/
    ${ECS_ROOT}/Component/
    ${ECS_ROOT}/Entity/
    ${ECS_ROOT}/System/
)

set(ECS_SRC
    ${ECS_ROOT}/Component/SparseArray.cpp
    ${ECS_ROOT}/Component/ComponentManager.cpp
    ${ECS_ROOT}/Entity/EntityManager.cpp
    ${ECS_ROOT}/System/SystemManager.cpp
)

include_directories(${ECS_INC})

add_library(${ECS} ${ECS_SRC})

install(TARGETS ${ECS})

################## BUILD IMGUI ##################

set(IMGUI IMGUI)

set(IMGUI_ROOT ${PROJECT_NAME}/IMGUI/)

set(IMGUI_INC
    ${IMGUI_ROOT}/
)

set(IMGUI_SRC
    ${IMGUI_ROOT}/imgui_draw.cpp
    ${IMGUI_ROOT}/imgui_tables.cpp
    ${IMGUI_ROOT}/imgui_widgets.cpp
    ${IMGUI_ROOT}/imgui.cpp
    ${IMGUI_ROOT}/imgui_impl_glfw.cpp
    ${IMGUI_ROOT}/imgui_impl_opengl3.cpp
)

include_directories(${IMGUI_INC})

add_library(${IMGUI} ${IMGUI_SRC})

install(TARGETS ${IMGUI})

################## BUILD GAME ##################

set(EXEC_NAME lush)

set(INC
    ${PROJECT_NAME}/
    ${PROJECT_NAME}/Graphic/
    ${PROJECT_NAME}/Systems/Camera/
    ${PROJECT_NAME}/Systems/Control/
    ${PROJECT_NAME}/Systems/Picking/
    ${PROJECT_NAME}/Systems/GUI/
    ${PROJECT_NAME}/Systems/Render/
    ${PROJECT_NAME}/IMGUI/
    ${PROJECT_NAME}/STB/
)

set(SRC
    ${PROJECT_NAME}/Engine.cpp
    ${PROJECT_NAME}/Graphic/Graphic.cpp
    ${PROJECT_NAME}/Graphic/RenderView.cpp
    ${PROJECT_NAME}/Graphic/Shader.cpp
    ${PROJECT_NAME}/Graphic/Mesh.cpp
    ${PROJECT_NAME}/Graphic/Model.cpp
    ${PROJECT_NAME}/Systems/Camera/CameraSystem.cpp
    ${PROJECT_NAME}/Systems/Control/ControlSystem.cpp
    ${PROJECT_NAME}/Systems/Picking/PickingSystem.cpp
    ${PROJECT_NAME}/Systems/GUI/GUISystem.cpp
    ${PROJECT_NAME}/Systems/Render/RenderSystem.cpp

    ${PROJECT_NAME}/STB/stb.cpp
)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
# LATER find_package(ASSIMP REQUIRED)

include_directories(${INC}
    ${OpenGL_INCLUDE_DIRS}
    ${GLFW3_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
)

add_executable(${EXEC_NAME}
    ${SRC}
    ${PROJECT_NAME}/main.cpp
)

target_link_libraries(${EXEC_NAME} PRIVATE
    ${CMAKE_THREAD_LIBS_INIT}
    ${OPENGL_LIBRARIES}
    glfw
    ${GLEW_LIBRARY}
    ${ECS}
    ${IMGUI}
)

target_link_libraries(${EXEC_NAME} PRIVATE assimp)
set_target_properties(${EXEC_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set_target_properties(${EXEC_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

#install target
install(TARGETS ${EXEC_NAME})

################## POST BUILD ##################
