#ifndef SCRIPT_HPP
#define SCRIPT_HPP

#include "Includes.hpp"
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/debug-helpers.h"
#include "mono/metadata/environment.h"
#include "mono/metadata/mono-config.h"

namespace Lush
{
    class Script
    {
        private:
            MonoDomain *_domain;
            MonoAssembly *_assembly;
            MonoImage *_image;

            MonoObject *_instance;
            MonoMethod *_onInit;
            MonoMethod *_onUpdate;

        public:
            Script(std::string name);
            ~Script();

            void loadScript(std::string name);

            void init();
            void update(float time);
    };
}

#endif // SCRIPT_HPP
