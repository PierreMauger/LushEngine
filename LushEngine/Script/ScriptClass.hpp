#ifndef SCRIPTCLASS_HPP
#define SCRIPTCLASS_HPP

#include "Includes.hpp"
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/debug-helpers.h"
#include "mono/metadata/environment.h"
#include "mono/metadata/mono-config.h"

namespace Lush
{
    class ScriptClass
    {
        private:
            MonoDomain *_domain;
            MonoAssembly *_assembly;
            MonoImage *_image;
            MonoClass *_class;
            MonoClass *_entityClass;

            std::map<std::string, MonoMethod *> _methods;

        public:
            ScriptClass(std::string name);
            ~ScriptClass();

            void loadScript(std::string name);

            MonoMethod *getMethod(std::string name);
            MonoDomain *getDomain();
            MonoClass *getClass();
    };
}

#endif // SCRIPTCLASS_HPP
