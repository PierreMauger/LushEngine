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
            MonoDomain *_domain;
            MonoAssembly *_assembly;
            MonoAssembly *_coreAssembly;
            MonoImage *_image;
            MonoImage *_coreImage;
            std::map<std::string, MonoClass *> _classes;
            MonoClass *_coreClass;

        public:
            ScriptPack(std::vector<File> &files, std::string name);
            ScriptPack() = default;
            ~ScriptPack() = default;

            void load(std::vector<File> &files);
            void reload(std::vector<File> &files);

            MonoDomain *getDomain();
            std::map<std::string, MonoClass *> &getClasses();
            MonoClass *getCoreClass();
    };
}

#endif // SCRIPTPACK_HPP
