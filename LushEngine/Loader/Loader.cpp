#include "Loader.hpp"

using namespace Lush;

Loader::Loader(std::shared_ptr<MessageBus> messageBus) : Node(messageBus)
{
    this->_launched = true; // loader always launched first, to launch others
    this->_loaderConfig = this->loadFile("resources/LoaderConfig.yaml");

    this->sendShaders();
    this->sendScenes();
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
    std::string copy = this->_loaderConfig;
    std::string result;

    while (std::regex_search(copy, match, regex)) {
        result += match[2];
        copy = match.suffix().str();
    }
    return result;
}

void Loader::sendShaders()
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
    this->sendMessage(Message(packet, RenderCommand::COMPILE_SHADER, Module::RENDER));
}

void Loader::sendScenes()
{
    Packet packet;
    std::vector<std::string> files = this->getFilesFromDir("resources/scenes/", false);
    std::string sceneConfig = this->searchInLoaderConfig("_Scene");

    if (sceneConfig.empty())
        throw std::runtime_error("Couldn't load Scene config.");

    // std::regex regex("\\s*(Default):\n*\\s*(.+\\b)");
    // std::smatch match;
    // bool defaultFound = false;
    // while (std::regex_search(sceneConfig, match, regex)) {
    //     if (match[1] == "Default") {
    //         if (std::find(files.begin(), files.end(), match[2]) == files.end())
    //             throw std::runtime_error("Scene not found: " + match[2].str());
    //         defaultFound = true;
    //     }
    //     sceneConfig = match.suffix().str();
    // }
    // if (!defaultFound)
    //     throw std::runtime_error("Default scene not found in config");

    for (auto &file : files) {
        std::regex regex("(\\d+)\\s*(\\S+)\\s*([+-]?[0-9]*[.]?[0-9]*)\\s*([+-]?[0-9]*[.]?[0-9]*)\\s*([+-]?[0-9]*[.]?[0-9]*)");
        std::smatch match;
        std::string fileContent = this->loadFile("resources/scenes/" + file);
        std::vector<GameObject> temp;

        while (std::regex_search(fileContent, match, regex)) {
            temp.push_back(GameObject(std::stoi(match[1]), match[2].str(),
                                      glm::vec3(std::stof(match[3]), std::stof(match[4]), std::stof(match[5]))));
            fileContent = match.suffix().str();
        }
        packet << file << temp.size();
        for (auto &object : temp)
            packet << object;
    }
    this->sendMessage(Message(packet, CoreCommand::SCENES, Module::CORE));
}
