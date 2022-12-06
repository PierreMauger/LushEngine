CMAKE_MINIMUM_REQUIRED(VERSION 3.20)
include(FetchContent)

FetchContent_Declare(LUABRIDGE GIT_REPOSITORY https://github.com/vinniefalco/LuaBridge.git)
FetchContent_MakeAvailable(LUABRIDGE)

set(LUABRIDGE_INCLUDE_DIR ${luabridge_SOURCE_DIR}/Source)
