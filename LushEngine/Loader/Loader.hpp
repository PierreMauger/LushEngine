#ifndef LOADER_HPP
#define LOADER_HPP

#include <filesystem>
#include <regex>

#include "Includes.hpp"
#include "Node.hpp"

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
            Packet loadShaders();
    };
}

#endif // LOADER_HPP
