#ifndef STATIC_MODEL_HPP
#define STATIC_MODEL_HPP

#include "Includes.hpp"
#include "Model.hpp"
#include "RenderObject.hpp"

namespace Lush
{
    class StaticModel : public RenderObject
    {
        private:
            std::shared_ptr<Model> _model;

        public:
            StaticModel(GameObject obj, std::shared_ptr<Model> model);
            ~StaticModel() = default;

            void draw(Camera &camera);
            bool showImGui(int id);
    };
}

#endif // STATIC_MODEL_HPP
