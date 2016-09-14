#ifndef __OPENGL_3D_RENDERABLE_H__
#define __OPENGL_3D_RENDERABLE_H__

#include <string>

namespace Nova{

    class Renderable {
    public:
        Renderable() {};
        virtual ~Renderable() {};
        virtual void load(std::string path) = 0;
        virtual void draw() = 0;
        
    };
}


typedef Nova::Renderable* Nova_CreateRenderable_t();
typedef void Nova_DestroyRenderable_t(Nova::Renderable*);

#endif
