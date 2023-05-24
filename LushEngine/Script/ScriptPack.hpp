#ifndef SCRIPT_PACK_HPP
#define SCRIPT_PACK_HPP

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
#include "mono/metadata/tokentype.h"

namespace Lush
{
    class ScriptPack : public Resource
    {
        private:
            std::string _name;

            MonoDomain *_domain = nullptr;
            MonoClass *_entityClass = nullptr;
            std::unordered_map<std::string, MonoClass *> _classes;

        public:
            ScriptPack(std::vector<File> &files, const std::string &name, MonoClass *entityClass);
            ScriptPack() = default;
            ~ScriptPack() = default;

            void load(std::vector<File> &files, MonoClass *entityClass);
            void reload(std::vector<File> &files, MonoClass *entityClass);
            void loadFromAssembly(const std::string &assemblyPath, MonoClass *entityClass);

            std::string getName() const;
            MonoDomain *getDomain();
            MonoClass *getEntityClass();
            std::unordered_map<std::string, MonoClass *> &getClasses();
    };
}

#endif // SCRIPT_PACK_HPP
