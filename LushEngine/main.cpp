#include "Engine.hpp"

#ifdef EDITOR_MODE
const bool editor = true;
#else
const bool editor = false;
#endif

int main()
{
    Lush::Engine engine(editor);

    engine.run();
    return 0;
}
