#include "Scene.hpp"

using namespace Lush;

Scene::Scene(std::map<std::size_t, GameObject> &objects)
{
    this->_objects = objects;
}

std::map<std::size_t, GameObject> &Scene::getObjects()
{
    return this->_objects;
}
