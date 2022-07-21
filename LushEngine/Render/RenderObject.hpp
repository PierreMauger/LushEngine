#ifndef RENDER_OBJECT_HPP
#define RENDER_OBJECT_HPP

#include "Includes.hpp"

namespace Lush
{
    class RenderObject
    {
        private:
            std::string _name;

        public:
            RenderObject();
            ~RenderObject();
            void draw();
    };
}

#endif // RENDER_OBJECT_HPP
