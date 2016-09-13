#include "ApplicationFactory.h"

#include <iostream>
#include <memory>
#include <nova/Rendering/OpenGL/Data_Structures/Model.h>

Nova::ApplicationFactory::ApplicationFactory( const Nova::Config& config ) : _config( config )
{
    _world = std::unique_ptr<OpenGL_World>( new OpenGL_World() );
    _world->Initialize(800,600);

    _shaderman = std::unique_ptr<ShaderManager>( new ShaderManager() );
}

void 
Nova::ApplicationFactory::Run()
{
    std::cout << "Loading scene from '" << _config.scenepath << "'" << std::endl; 

    Shader shader("shader.vs","shader.frag");
    _world->Set_Shader(shader);
    Model model("nanosuit/nanosuit.obj");
    _world->Main_Loop(model);

}
