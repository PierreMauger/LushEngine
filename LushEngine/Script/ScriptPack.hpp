#ifndef SCRIPTPACK_HPP
#define SCRIPTPACK_HPP

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
    class ScriptPack : public Resource
    {
        private:
            std::string _name;
            std::vector<std::string> _linkedPacks;

            MonoDomain *_domain;
            MonoClass *_entityClass;
            std::unordered_map<std::string, MonoClass *> _classes;

        public:
            ScriptPack(std::vector<File> &files, std::string name, std::unordered_map<std::string, ScriptPack> &corePacks);
            ScriptPack() = default;
            ~ScriptPack() = default;

            void load(std::vector<File> &files, std::unordered_map<std::string, ScriptPack> &corePacks);
            void reload(std::vector<File> &files, std::unordered_map<std::string, ScriptPack> &corePacks);

            std::string getName() const;
            const std::vector<std::string> &getLinkedPacks() const;
            MonoDomain *getDomain();
            MonoClass *getEntityClass();
            std::unordered_map<std::string, MonoClass *> &getClasses();
    };
}

#endif // SCRIPTPACK_HPP
