#include "Loader.hpp"

using namespace Lush;

Loader::Loader(std::shared_ptr<MessageBus> messageBus) : Node(messageBus)
{
    this->_launched = true; // loader always launched first, to launch others
    this->_loaderConfig = this->loadFile("resources/LoaderConfig.yaml");

    this->sendMessage(Message(this->loadShaders(), RenderCommand::LOAD_SHADER, Module::RENDER));
}

Loader::~Loader()
{
}

void Loader::run()
{
    this->sendMessage(Message(Packet(), BaseCommand::LAUNCH, Module::BROADCAST));

    while (this->_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        this->_messageBus->notify(Module::LOADER, this->_functionList);
        if (!this->_launched)
            continue;
    }
}

std::vector<std::string> Loader::getFilesFromDir(std::string dir, bool withPath)
{
    std::vector<std::string> files;

    for (auto &file : std::filesystem::directory_iterator(dir))
        if (withPath)
            files.push_back(file.path().string());
        else
            files.push_back(file.path().filename().string());
    return files;
}

std::string Loader::loadFile(std::string fileName)
{
    std::ifstream file;
    std::string buffer;
    std::stringstream stream;

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(fileName);
        stream << file.rdbuf();
        file.close();
        buffer = stream.str();
    } catch (std::ifstream::failure &e) {
        throw std::runtime_error("File loading error: " + fileName);
    }
    return buffer;
}

std::string Loader::searchInLoaderConfig(std::string section)
{
    std::regex regex("(" + section + ":\n)([^_]*)");
    std::smatch match;
    std::string result;

    while (std::regex_search(this->_loaderConfig, match, regex)) {
        result += match[2];
        this->_loaderConfig = match.suffix().str();
    }
    return result;
}

Packet Loader::loadShaders()
{
    Packet packet;
    std::vector<std::string> files = this->getFilesFromDir("resources/shaders/", false);
    std::string shaderConfig = this->searchInLoaderConfig("_Shader");

    if (shaderConfig.empty())
        throw std::runtime_error("Couldn't load Shader config.");

    std::regex regex("\\s*(.+):\n*\\s*(.+\\.vs\\b)\n*\\s*(.+\\.fs\\b)");
    std::smatch match;
    while (std::regex_search(shaderConfig, match, regex)) {
        if (std::find(files.begin(), files.end(), match[2]) == files.end())
            throw std::runtime_error("Shader config vertex not found: " + match[2].str());
        if (std::find(files.begin(), files.end(), match[3]) == files.end())
            throw std::runtime_error("Shader config fragment not found: " + match[3].str());

        packet << match[1].str() << this->loadFile("resources/shaders/" + match[2].str())
               << this->loadFile("resources/shaders/" + match[3].str());
        shaderConfig = match.suffix().str();
    }
    return packet;
}
