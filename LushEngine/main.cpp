#include "Engine.hpp"
#include "Includes.hpp"

static Lush::Engine *engine = nullptr;

Lush::Engine *Lush::Engine::getEngine()
{
    return engine;
}

int main()
{
    engine = new Lush::Engine();

    engine->run();
    delete engine;
    return 0;
}
