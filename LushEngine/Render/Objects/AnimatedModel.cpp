#include "AnimatedModel.hpp"

using namespace Lush;

AnimatedModel::AnimatedModel(GameObject obj, std::shared_ptr<Model> model) : RenderObject(obj)
{
    this->_model = model;
    // animator and animations
}

void AnimatedModel::draw([[maybe_unused]] Camera &camera)
{
}

bool AnimatedModel::showImGui([[maybe_unused]] int id)
{
    return false;
}
