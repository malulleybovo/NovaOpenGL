#ifndef __OPENGL_3D_RENDERABLE_MANAGER_H__
#define __OPENGL_3D_RENDERABLE_MANAGER_H__

#include <string>
#include <map>
#include <memory>
#include <vector>

namespace Nova {

    class ApplicationFactory;
    class Renderable;

    class Renderable {
    public:
    Renderable(ApplicationFactory& app) : _app(app) {};
      virtual ~Renderable() {};
      virtual void draw() = 0;
    protected:
      ApplicationFactory& _app;
    };
    
    class RenderableFactory{
    public:
      RenderableFactory() {};
      virtual ~RenderableFactory() {};
      
      virtual std::unique_ptr<Renderable> Create(ApplicationFactory& app, std::string path ) = 0;
      virtual bool AcceptExtension(std::string ext) const = 0;
    };
    
    class RenderableManager {
    public:
        RenderableManager( ApplicationFactory& app );
        virtual ~RenderableManager();

        unsigned long RegisterRenderable( std::string path );
	void AddFactory( std::unique_ptr<RenderableFactory> );
	  
        Renderable* Get(unsigned long id);
        unsigned long MaxId();
        
    private:
        ApplicationFactory& _app;

        unsigned long _resource_counter;
        std::map< unsigned long, std::unique_ptr<Renderable> >  _resources;
	std::vector< std::unique_ptr<RenderableFactory> > _factories;
    };

}


#endif
