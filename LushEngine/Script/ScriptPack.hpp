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
            std::unordered_map<std::string, MonoClass *> _classes;

        public:
            ScriptPack(std::vector<File> &files, const std::string &name);
            ScriptPack() = default;
            ~ScriptPack() = default;

            void load(std::vector<File> &files);
            void reload(std::vector<File> &files);
            void loadFromAssembly(const std::string &assemblyPath);

            std::string getName() const;
            MonoDomain *getDomain();
            std::unordered_map<std::string, MonoClass *> &getClasses();
    };
}

#endif // SCRIPT_PACK_HPP
