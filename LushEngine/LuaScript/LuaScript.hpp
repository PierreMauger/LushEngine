#include <lua.hpp>

#include "Includes.hpp"
#include "LuaBridge/LuaBridge.h"

namespace Lush
{
    class LuaScript
    {
        private:
            lua_State *_luaState;

        public:
            LuaScript(std::string path);
            ~LuaScript();

            void loadScript(std::string path);
            void init();
            void update();
    };
}
