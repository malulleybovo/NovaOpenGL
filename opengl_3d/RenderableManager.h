#ifndef __OPENGL_3D_RENDERABLE_MANAGER_H__
#define __OPENGL_3D_RENDERABLE_MANAGER_H__

namespace Nova {

    class RenderableManager {
    public:
        RenderableManager( ApplicationFactory& app );
        virtual ~RenderableManager();

        void SetPluginSearchPath( std::string searchpath );        

        void RegisterPlugin( std::string extension, std::string plugin );
        unsigned long RegisterRenderable( std::string path );

        Renderable* Get(unsigned long id);
        
    private:
        ApplicationFactory& _app;

        struct RenderableRecord {
            Renderable* resource;
            std::string plugin;
        };

        unsigned long _resource_counter;
        std::map< unsigned long, RenderableRecord >  _resources;
        std::map< std::string, std::unique_ptr<Plugin> > _plugins;
    };

}


#endif
