#include "ApplicationFactory.h"

#include <iostream>
#include <memory>
#include <nova/Rendering/OpenGL/Data_Structures/Model.h>

Nova::ApplicationFactory::ApplicationFactory( const Nova::Config& config ) : _config( config )
{
//    _world = std::unique_ptr<OpenGL_World>( new OpenGL_World() );
//    _world->Initialize(800,600);
    _scene = std::unique_ptr<Scene>( new Scene(*this) );
    _shaderman = std::unique_ptr<ShaderManager>( new ShaderManager(*this) );
    _pluginman = std::unique_ptr<PluginManager>( new PluginManager(*this) );
    _pluginman->SetSearchPaths( _config.pluginpaths );
}

Nova::ShaderManager&
Nova::ApplicationFactory::GetShaderManager()
{ return *(_shaderman.get()); }

Nova::PluginManager&
Nova::ApplicationFactory::GetPluginManager()
{ return *(_pluginman.get()); }

Nova::Scene&
Nova::ApplicationFactory::GetScene()
{ return *(_scene.get()); }

void 
Nova::ApplicationFactory::Run()
{
    std::cout << "Loading scene from '" << _config.scenepath << "'" << std::endl; 
    _scene->Configure( _config.scenepath );
    _scene->Load();

//    Shader shader("shader.vs","shader.frag");
//    _world->Set_Shader(shader);
//    Model model("nanosuit/nanosuit.obj");
//    _world->Main_Loop(model);

}
