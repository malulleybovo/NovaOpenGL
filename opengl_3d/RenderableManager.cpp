#include "RenderableManager.h"
#include "ApplicationFactory.h"
#include "Renderable.h"
#include <exception>
#include <limits>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

Nova::RenderableManager::RenderableManager( ApplicationFactory& app ) : _app(app), _resource_counter(0)
{

}

Nova::RenderableManager::~RenderableManager(){
    for( auto iter = _resources.begin(); iter != _resources.end(); iter++){
        RenderableRecord& record = iter->second;
        if( record.resource != nullptr ){ // Need to clean stuff up...
            Plugin* plugin = _app.GetPluginManager().Get( record.plugin );
            plugin->Destroy( record.resource );
            record.resource = nullptr;            
        }
    }
}

unsigned long
Nova::RenderableManager::MaxId()
{
    return  std::numeric_limits<unsigned long>::max();
}

void
Nova::RenderableManager::RegisterExtension( std::string extension, std::string plugin )
{
    auto res = _extmap.find( extension );
    if( res != _extmap.end() && res->second != plugin ){
        throw std::runtime_error(std::string("Extension already registered with plugin ") + res->second );
    }
    else if( res == _extmap.end() ){
        _extmap.insert( std::make_pair( extension, plugin ) );        
    }    
}

unsigned long
Nova::RenderableManager::RegisterRenderable( std::string path )
{
    fs::path renderablePath;
    renderablePath /= path;
    try{
        renderablePath = fs::canonical( renderablePath );
    }
    catch( std::exception& e ){
        throw std::runtime_error(std::string("Could not validate Renderable path: ") + e.what() );
    }

    if( renderablePath.has_extension() ){
        std::string ext = renderablePath.extension().native();
        ext = ext.substr(1); // Strip leading '.'
        auto res =  _extmap.find( ext );
        if( res != _extmap.end() ){
            RenderableRecord record;
            record.path = path;
            record.resource = nullptr;
            record.plugin = res->second;
            unsigned long id = _resource_counter++;
            _resources.insert( std::make_pair( id, record ) );
            return id;
        }
        else{
            throw std::runtime_error( std::string("No plugin found to handle file with extension: ") + ext );
        }
    }
    else
        throw std::runtime_error( "Could not register Renderable. No extension found." );

}

Nova::Renderable* 
Nova::RenderableManager::Get( unsigned long id )
{
    auto res = _resources.find( id );
    if( res != _resources.end() ){
        RenderableRecord& record = res->second;
        if( record.resource == nullptr ){ // Resource is not currently loaded. Need to get the resource
            Plugin* plugin = _app.GetPluginManager().Get( record.plugin );
            record.resource = plugin->Create(_app);
            record.resource->load( record.path );            
        }
        return record.resource;
    }
    else{
        throw std::runtime_error( "No resource matching the specified id was registered." );
    }
    
}


