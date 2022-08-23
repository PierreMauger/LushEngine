#ifndef SCENE_HPP
#define SCENE_HPP

#include "GameObject.hpp"
#include "Includes.hpp"

namespace Lush
{
    class Scene
    {
        private:
            std::map<std::size_t, GameObject> _objects;

        public:
            Scene(std::map<std::size_t, GameObject> &objects);
            Scene() = default;
            ~Scene() = default;

            std::map<std::size_t, GameObject> &getObjects();
    };
}

#endif // SCENE_HPP
