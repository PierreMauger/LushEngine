#ifndef ANIMATED_MODEL_HPP
#define ANIMATED_MODEL_HPP

#include "Includes.hpp"
#include "Model.hpp"
#include "RenderObject.hpp"

namespace Lush
{
    class AnimatedModel : public RenderObject
    {
        private:
            std::shared_ptr<Model> _model;

        public:
            AnimatedModel(GameObject obj, std::shared_ptr<Model> model);
            ~AnimatedModel() = default;

            void draw(Camera &camera);
            void showImGui(int id);
    };
}

#endif // ANIMATED_MODEL_HPP
