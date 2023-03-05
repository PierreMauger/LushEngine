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
            MonoAssembly *_assembly;
            MonoAssembly *_coreAssembly;
            MonoImage *_image;
            MonoImage *_coreImage;
            std::unordered_map<std::string, MonoClass *> _classes;
            MonoClass *_coreClass;

        public:
            ScriptPack(std::vector<File> &files, std::string name, std::unordered_map<std::string, ScriptPack> &corePacks);
            ScriptPack() = default;
            ~ScriptPack() = default;

            void load(std::vector<File> &files, std::unordered_map<std::string, ScriptPack> &corePacks);
            void reload(std::vector<File> &files, std::unordered_map<std::string, ScriptPack> &corePacks);

            std::string getName() const;
            MonoDomain *getDomain();
            std::unordered_map<std::string, MonoClass *> &getClasses();
            MonoClass *getCoreClass();
    };
}

#endif // SCRIPTPACK_HPP
