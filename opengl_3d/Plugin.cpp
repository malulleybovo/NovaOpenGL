#include "Plugin.h"
#include "Renderable.h"
#include <dlfcn.h>
#include <iostream>

Nova::Plugin::Plugin( std::string name, std::string path ) : _name( name )
{
    const char *error;

    _handle = dlopen( path.c_str(), RTLD_LAZY );
    if( !_handle ){
        std::cerr << "Cannot load library: " << dlerror() << std::endl;
        throw 1;
    }

    dlerror();
    Nova_CreateRenderable_t* create_renderable = (Nova_CreateRenderable_t*) dlsym( _handle, "Nova_CreateRenderable");
    if( (error = dlerror()) ){
        std::cerr << "Could not find symbol in " << _name << " plugin: " << error << std::endl;
        throw 2;
    }


    dlerror();
    Nova_DestroyRenderable_t* destroy_renderable = (Nova_DestroyRenderable_t*) dlsym( _handle, "Nova_DestroyRenderable");
    if( (error = dlerror()) ){
        std::cerr << "Could not find symbol in " << _name << " plugin: " << error << std::endl;
        throw 2;
    }

        
}

Nova::Plugin::~Plugin()
{
    dlclose(_handle);
}

Nova::Renderable*
Nova::Plugin::Create(ApplicationFactory& app)
{
    const char *error;
    dlerror();
    Nova_CreateRenderable_t* create_renderable = (Nova_CreateRenderable_t*) dlsym( _handle, "Nova_CreateRenderable");
    if( (error = dlerror()) ){
        std::cerr << "Could not find symbol in " << _name << " plugin: " << error << std::endl;
        throw 2;
    }
    
    return create_renderable(app);        
}


void 
Nova::Plugin::Destroy( Nova::Renderable* renderable )
{
    const char *error;
    dlerror();
    Nova_DestroyRenderable_t* destroy_renderable = (Nova_DestroyRenderable_t*) dlsym( _handle, "Nova_DestroyRenderable");
    if( (error = dlerror()) ){
        std::cerr << "Could not find symbol in " << _name << " plugin: " << error << std::endl;
        throw 2;
    }

    destroy_renderable( renderable );

}
