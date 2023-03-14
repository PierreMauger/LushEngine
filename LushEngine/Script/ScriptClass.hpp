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
    typedef struct {
        std::string type;
        MonoClassField *field;
    } FieldInfo;

    class ScriptClass
    {
        private:
            MonoDomain *_domain;
            MonoClass *_class;

            std::unordered_map<std::string, MonoMethod *> _methods;
            std::unordered_map<std::string, FieldInfo> _fields;

        public:
            ScriptClass(MonoDomain *domain, MonoClass *sciptClass, MonoClass *entityClass);
            ScriptClass() = default;
            ~ScriptClass() = default;

            void load(MonoDomain *domain, MonoClass *sciptClass, MonoClass *entityClass);
            void loadAttributes();
            void reload(MonoDomain *domain, MonoClass *sciptClass, MonoClass *entityClass);

            MonoMethod *getMethod(std::string name);
            std::unordered_map<std::string, FieldInfo> &getFields();

            MonoDomain *getDomain();
            MonoClass *getClass();
    };
}

#endif // SCRIPTCLASS_HPP
