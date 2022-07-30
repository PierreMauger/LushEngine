#include "StaticModel.hpp"

using namespace Lush;

StaticModel::StaticModel(GameObject obj, std::shared_ptr<Model> model) : RenderObject(obj)
{
    this->_model = model;
}

void StaticModel::draw(Camera &camera)
{
    camera.setOnModel(this->_position, this->_scale, this->_rotation);
    this->_model->draw(camera.getShader());
}
