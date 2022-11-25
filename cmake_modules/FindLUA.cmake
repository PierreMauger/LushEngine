cmake_minimum_required(VERSION 3.10)
include(ExternalProject)

ExternalProject_Add(lua
    URL "https://www.lua.org/ftp/lua-5.4.4.tar.gz"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND make generic 2> /dev/null
    BUILD_ALWAYS true
    BUILD_IN_SOURCE true
    INSTALL_COMMAND ""
    DOWNLOAD_EXTRACT_TIMESTAMP 0
)
add_library(liblua STATIC IMPORTED)
ExternalProject_Get_property(lua SOURCE_DIR)

set(LUA_INCLUDE_DIR ${SOURCE_DIR}/src)
set(LUA_LIBRARIES ${SOURCE_DIR}/src/liblua.a)
