#ifndef __OPENGL_3D_APPLICATION_FACTORY_H__
#define __OPENGL_3D_APPLICATION_FACTORY_H__

#include "Config.h"
#include <memory>

#include <nova/Rendering/OpenGL/World/OpenGL_World.h>
#include "ShaderManager.h"
#include "Scene.h"
#include "PluginManager.h"

namespace Nova{

    class ApplicationFactory {
        public:
            ApplicationFactory( const Config& config );
            void Run();

            ShaderManager& GetShaderManager();
            PluginManager& GetPluginManager();
            Scene&         GetScene();

        private:        
            Config _config;
//std::unique_ptr<OpenGL_World> _world;
            std::unique_ptr<Scene> _scene;
            std::unique_ptr<ShaderManager> _shaderman;
            std::unique_ptr<PluginManager> _pluginman;
    };
}

#endif
