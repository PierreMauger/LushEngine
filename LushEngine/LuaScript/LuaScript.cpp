#include "LuaScript/LuaScript.hpp"

using namespace Lush;

LuaScript::LuaScript(std::string path)
{
    this->_luaState = luaL_newstate();
    luaL_openlibs(this->_luaState);

    this->loadScript(path);
    // lua_close(this->_luaState);
}

LuaScript::~LuaScript()
{
}

void LuaScript::loadScript(std::string path)
{
    int ret = luaL_dofile(this->_luaState, path.c_str());
    if (ret != LUA_OK) {
        std::cout << "Error: " << lua_tostring(this->_luaState, -1) << std::endl;
        lua_pop(this->_luaState, 1);
    }
}

void LuaScript::init()
{
    luabridge::LuaRef init = luabridge::getGlobal(this->_luaState, "onInit");
    if (init.isFunction())
        init();
}

void LuaScript::update()
{
    int ret = lua_getglobal(this->_luaState, "onUpdate");
    if (lua_isfunction(this->_luaState, -1)) {
        lua_pcall(this->_luaState, 0, 0, 0);
        // ret = lua_tonumber(this->_luaState, -1);
        // std::cout << "LuaScript::update: " << ret << std::endl;
        // ret = lua_tonumber(this->_luaState, -2);
        // std::cout << "LuaScript::update: " << ret << std::endl;
    }

    // luabridge::LuaRef update = luabridge::getGlobal(this->_luaState, "onUpdate");
    // if (update.isFunction()) {
        // update();
        //
        // int ret = lua_tonumber(this->_luaState, -1);

        // std::cout << "LuaScript::update: " << ret << std::endl;
        // std::cout << "LuaScript::update: " << luabridge::Stack<int>::get(this->_luaState, -1) << std::endl;

    // }
}
