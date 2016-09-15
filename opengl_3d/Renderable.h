#ifndef __OPENGL_3D_RENDERABLE_H__
#define __OPENGL_3D_RENDERABLE_H__

#include <string>

namespace Nova{

    class ApplicationFactory;

    class Renderable {
    public:
        Renderable(ApplicationFactory& app) : _app(app) {};
        virtual ~Renderable() {};
        virtual void load(std::string path) = 0;
        virtual void draw() = 0;
    protected:
        ApplicationFactory& _app;
    };
}


typedef Nova::Renderable* Nova_CreateRenderable_t(Nova::ApplicationFactory& app);
typedef void Nova_DestroyRenderable_t(Nova::Renderable*);

#endif
