#include "ApplicationFactory.h"
#include <iostream>
#include <memory>

Nova::ApplicationFactory::ApplicationFactory( const Nova::Config& config ) : _config( config )
{
    _ioservice = std::unique_ptr<IOService>( new IOService(*this) );
    _world = std::unique_ptr<World>( new World(*this) );    
    _scene = std::unique_ptr<Scene>( new Scene(*this) );
    _shaderman = std::unique_ptr<ShaderManager>( new ShaderManager(*this) );
    _pluginman = std::unique_ptr<PluginManager>( new PluginManager(*this) );
    _pluginman->AppendSearchPaths( _config.pluginpaths );
    _renderableman = std::unique_ptr<RenderableManager>( new RenderableManager(*this) );
    _textrenderingservice = std::unique_ptr<TextRenderingService>( new TextRenderingService(*this) );
    _commanddispatch = std::unique_ptr<CommandDispatch>( new CommandDispatch(*this) );
}

Nova::IOService&
Nova::ApplicationFactory::GetIOService()
{ return *(_ioservice.get()); }

Nova::ShaderManager&
Nova::ApplicationFactory::GetShaderManager()
{ return *(_shaderman.get()); }

Nova::PluginManager&
Nova::ApplicationFactory::GetPluginManager()
{ return *(_pluginman.get()); }

Nova::RenderableManager&
Nova::ApplicationFactory::GetRenderableManager()
{ return *(_renderableman.get()); }

Nova::Scene&
Nova::ApplicationFactory::GetScene()
{ return *(_scene.get()); }

Nova::World&
Nova::ApplicationFactory::GetWorld()
{ return *(_world.get()); }

Nova::TextRenderingService&
Nova::ApplicationFactory::GetTextRenderingService()
{ return *(_textrenderingservice.get()); }

Nova::CommandDispatch&
Nova::ApplicationFactory::GetCommandDispatch()
{ return *(_commanddispatch.get()); }

void 
Nova::ApplicationFactory::Run()
{
    GetIOService().On( IOService::REDRAW, [&](IOEvent& event){_textrenderingservice->UpdateScreenSize( event ); });
    GetIOService().On( IOService::KEY_DOWN, [&](IOEvent& event){ _commanddispatch->Input( event ); });
    GetIOService().On( IOService::TIME, [&](IOEvent& event){  _commanddispatch->Render( event ); });
    
    _world->Initialize(800,600);
    std::cout << "Loading scene from '" << _config.scenepath << "'" << std::endl; 
    _scene->Configure( _config.scenepath );
    _scene->Load();
    _world->Main_Loop();
}
