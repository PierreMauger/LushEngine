#include "Engine.hpp"
#include "Systems/FileWatcher/FileWatcherSystem.hpp"
#include "Systems/GUI/GUISystem.hpp"
#include "Systems/Picking/PickingSystem.hpp"
#include "Systems/PostProcessing/PostProcessingSystem.hpp"
#include "Systems/Scene/SceneSystem.hpp"

#ifdef EDITOR_MODE
const bool editor = true;
#else
const bool editor = false;
#endif

int main()
{
    Lush::Engine engine(editor);
#ifdef EDITOR_MODE
    engine.bindSystem<Lush::SceneSystem>();
    engine.bindSystem<Lush::PickingSystem>();
    engine.bindSystem<Lush::GUISystem>();
    engine.bindSystem<Lush::FileWatcherSystem>();
#else
    engine.bindSystem<Lush::PostProcessingSystem>();
#endif

    engine.run();
    return 0;
}
