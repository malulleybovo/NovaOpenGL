#ifndef __OPENGL_3D_RENDERABLE_MANAGER_H__
#define __OPENGL_3D_RENDERABLE_MANAGER_H__

#include <string>
#include <map>
#include <memory>

namespace Nova {

    class ApplicationFactory;
    class Renderable;

    class RenderableManager {
    public:
        RenderableManager( ApplicationFactory& app );
        virtual ~RenderableManager();

        void RegisterExtension( std::string extension, std::string plugin );
        unsigned long RegisterRenderable( std::string path );

        Renderable* Get(unsigned long id);
        unsigned long MaxId();
        
    private:
        ApplicationFactory& _app;

        struct RenderableRecord {
            std::string path;
            Renderable* resource;
            std::string plugin;
        };

        unsigned long _resource_counter;
        std::map< unsigned long, RenderableRecord >  _resources;
        std::map< std::string, std::string > _extmap;
    };

}


#endif
