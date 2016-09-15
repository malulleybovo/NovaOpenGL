#ifndef __OPENGL_3D_SCENE_H__
#define __OPENGL_3D_SCENE_H__

#include <string>
#include <vector>

namespace Nova{
    
    class ApplicationFactory;

    class Scene {
    public:
        Scene(ApplicationFactory& app);
        void Configure(std::string path);
        void Load();
        void Draw(); // OpenGL calls go here!
        
    private:
        ApplicationFactory& _app;
        std::string _path;
        std::vector< unsigned long > _scene_objects;
    };

}


#endif
