#ifndef __OPENGL_3D_PLUGIN_H__
#define __OPENGL_3D_PLUGIN_H__

#include "Renderable.h"

#include <string>

namespace Nova {

    class Plugin {
    public:
        Plugin( std::string name, std::string path );
        ~Plugin();

        Renderable* Create(ApplicationFactory& app);
        void Destroy( Renderable* renderable );

    private:
        void* _handle;
        std::string _name;
    
    };
}

#endif
