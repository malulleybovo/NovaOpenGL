#ifndef __OPENGL_3D_SCENE_H__
#define __OPENGL_3D_SCENE_H__

#include <string>
#include <map>

namespace Nova{
    
    class ApplicationFactory;

    class Scene {
    public:
        Scene(ApplicationFactory& app);
        void Configure(std::string path);
        void Load();
        
    private:
        ApplicationFactory& _app;
        std::string _path;
        std::map< std::string, std::string > _known_extensions;

    };

}


#endif
