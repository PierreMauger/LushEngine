#ifndef SCRIPTCLASS_HPP
#define SCRIPTCLASS_HPP

#include "File/File.hpp"
#include "File/Resource.hpp"
#include "Includes.hpp"
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/debug-helpers.h"
#include "mono/metadata/environment.h"
#include "mono/metadata/mono-config.h"
#include "mono/metadata/threads.h"

namespace Lush
{
    class ScriptClass : Resource
    {
        private:
            std::string _name;
            MonoDomain *_domain;
            MonoAssembly *_assembly;
            MonoAssembly *_entityAssembly;
            MonoImage *_image;
            MonoImage *_entityImage;
            MonoClass *_class;
            MonoClass *_entityClass;

            std::map<std::string, MonoMethod *> _methods;

        public:
            ScriptClass(File &file);
            ScriptClass() = default;
            ~ScriptClass() = default;

            void load(std::string name);
            void reload();

            MonoMethod *getMethod(std::string name);
            MonoDomain *getDomain();
            MonoClass *getClass();
    };
}

#endif // SCRIPTCLASS_HPP
