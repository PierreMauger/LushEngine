#ifndef SCRIPTCLASS_HPP
#define SCRIPTCLASS_HPP

#include "File/File.hpp"
#include "File/Resource.hpp"
#include "Includes.hpp"
#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/attrdefs.h"
#include "mono/metadata/debug-helpers.h"
#include "mono/metadata/environment.h"
#include "mono/metadata/metadata.h"
#include "mono/metadata/mono-config.h"
#include "mono/metadata/threads.h"

namespace Lush
{
    class ScriptClass : public Resource
    {
        private:
            MonoDomain *_domain;
            MonoAssembly *_assembly;
            MonoAssembly *_entityAssembly;
            MonoImage *_image;
            MonoImage *_entityImage;
            MonoClass *_class;
            MonoClass *_entityClass;

            std::map<std::string, MonoMethod *> _methods;
            std::map<std::string, std::string> _attributes;

        public:
            ScriptClass(File &file);
            ScriptClass() = default;
            ~ScriptClass() = default;

            void load(File &file);
            void reload(File &file);
            void loadAttributes();

            MonoMethod *getMethod(std::string name);
            MonoDomain *getDomain();
            MonoClass *getClass();
    };
}

#endif // SCRIPTCLASS_HPP
