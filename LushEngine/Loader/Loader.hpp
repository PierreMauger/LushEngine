#ifndef LOADER_HPP
#define LOADER_HPP

#include <filesystem>
#include <regex>

#include "Includes.hpp"
#include "GameObject.hpp"
#include "Node.hpp"

#define REGEX_VEC3 "\\(([+-]?[0-9]*[.]?[0-9]*)\\s*([+-]?[0-9]*[.]?[0-9]*)\\s*([+-]?[0-9]*[.]?[0-9]*)\\)"

namespace Lush
{
    class Loader : public virtual Node
    {
        private:
            std::string _loaderConfig;

        public:
            Loader(std::shared_ptr<MessageBus> messageBus);
            ~Loader();
            void run();

            std::vector<std::string> getFilesFromDir(std::string dir, bool withPath = true);
            std::string loadFile(std::string fileName);
            std::string searchInLoaderConfig(std::string section);

        private:
            void sendShaders();
            void sendTextures();
            void sendModels();
            void sendScenes();
            void sendIcon();
            void sendSkyBox();
    };
}

#endif // LOADER_HPP
