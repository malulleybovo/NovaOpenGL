#ifndef __OPENGL_3D_APPLICATION_FACTORY_H__
#define __OPENGL_3D_APPLICATION_FACTORY_H__

#include "Config.h"
#include <memory>

#include "ShaderManager.h"
#include "Scene.h"
#include "PluginManager.h"
#include "RenderableManager.h"
#include "World.h"

namespace Nova{


    constexpr int API_VERSION = 1;
  
    class ApplicationFactory {
        public:
            ApplicationFactory( const Config& config );
            void Run();

            ShaderManager& GetShaderManager();
            PluginManager& GetPluginManager();
            RenderableManager& GetRenderableManager();
            Scene&         GetScene();
            World&         GetWorld();
	    
        private:        
            Config _config;
            std::unique_ptr<World> _world;
            std::unique_ptr<Scene> _scene;
            std::unique_ptr<ShaderManager> _shaderman;
            std::unique_ptr<PluginManager> _pluginman;
            std::unique_ptr<RenderableManager> _renderableman;
    };
}

#endif
