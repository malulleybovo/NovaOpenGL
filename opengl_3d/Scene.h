#ifndef __OPENGL_3D_SCENE_H__
#define __OPENGL_3D_SCENE_H__

#include <string>
#include <vector>
#include <memory>
#include "Shader.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace Nova{
    
    class ApplicationFactory;
    class IOEvent;

    class Scene {
    public:
        Scene(ApplicationFactory& app);
        void Configure(std::string path);
        void Load();
        void Selection( IOEvent& event );
        void Draw(); // OpenGL calls go here!
        
    private:
        bool _debugHitSelection;
        std::unique_ptr<Shader> lineShader;
        glm::vec3 hit_start;
        glm::vec3 hit_end;
        unsigned long selected_object;
        ApplicationFactory& _app;
        std::string _path;
        std::vector< unsigned long > _scene_objects;
    };

}


#endif
