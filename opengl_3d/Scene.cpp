#include "Scene.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include <string>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <utility>

#include "ApplicationFactory.h"

Nova::Scene::Scene(ApplicationFactory& app) : _app(app)
{

}

std::vector< std::string > 
tokenize( std::string str ){
    typedef std::string::size_type size_type;
    std::vector< std::string > tokens;
    size_type pos = 0;
    while( pos != std::string::npos ){
        size_type next_token = str.find_first_of(".", pos);
        if( next_token != std::string::npos ){
            tokens.push_back( str.substr( pos, next_token-pos ) );
            pos = next_token+1;
        }
        else{
            tokens.push_back( str.substr( pos ) );
            pos = std::string::npos;
        }
    }                         
    return tokens;
}

void
Nova::Scene::Configure( std::string path ){

    _path = path;

    fs::path config_path;
    config_path += path;
    config_path += "scene.conf";

    po::options_description config("Configuration");
    config.add_options()
        ;
    
    std::ifstream ifs(config_path.c_str());
    if (!ifs)
        return;

    po::variables_map vm;
    auto results = parse_config_file(ifs, config, true);
    store( results, vm );
    std::vector<std::string> unrecognized_results = po::collect_unrecognized( results.options, po::exclude_positional );
    notify(vm);        

    std::cout << "Extension Details: " << std::endl;
    for( int i = 0; i < unrecognized_results.size(); i+=2){
        std::vector<std::string> entryList = tokenize( unrecognized_results[i] );
        
        std::string key = "extension";
        if( entryList[0] != key )
            continue;
        
        std::string extension = entryList[1];                                                               
        std::string plugin = unrecognized_results[i+1];
        std::cout << extension << " : " << plugin << std::endl;
        _app.GetRenderableManager().RegisterExtension( extension, plugin );
    }


}

void
Nova::Scene::Load()
{
    fs::path load_path;
    load_path /= _path;
    load_path = fs::canonical(load_path);
    std::cout << "Loading " << load_path.native() << std::endl;
    
    fs::directory_iterator diter( load_path );
    for( ; diter != fs::directory_iterator(); diter++){
        unsigned long objectId = _app.GetRenderableManager().MaxId();
        try{
            objectId = _app.GetRenderableManager().RegisterRenderable( diter->path().native() );
        }
        catch( std::exception& e ){
            // We failed to load this particular file, say something about it...
            //std::cout << "Couldn't load " << diter->path().native() << ", Reason: " << std::endl;
            //std::cout << e.what() << std::endl;
        }

        if( objectId != _app.GetRenderableManager().MaxId() ){
            _scene_objects.push_back( objectId );
            std::cout << "Loaded " << diter->path().filename() << " with id " << objectId << std::endl;
        }
    }

}


void Nova::Scene::Draw()
{
    for( auto objectId : _scene_objects ){
        Renderable* renderable = _app.GetRenderableManager().Get( objectId );
        renderable->draw();
    }
}
