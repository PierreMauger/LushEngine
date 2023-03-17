#include "Engine.hpp"
#include "Includes.hpp"

int main(int argc, char **argv)
{
    Lush::Engine engine(argc > 1 && std::string(argv[1]) == "--editor");

    engine.run();
    return 0;
}
