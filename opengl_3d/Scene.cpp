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
        _known_extensions.insert( std::make_pair( extension, plugin ) );
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
        if( diter->path().has_extension() ){
            std::string ext = diter->path().extension().native();
            ext = ext.substr(1); // Strip leading '.'
            auto res =  _known_extensions.find( ext );
            if( res != _known_extensions.end() ){
                std::cout << "Loading " << diter->path().native() << std::endl;
                Plugin* plugin = nullptr;
                try{
                    plugin = _app.GetPluginManager().Get( res->second );
                }
                catch( std::runtime_error e ){
                    std::cout << "Failed to load plugin for " << diter->path().filename() << std::endl;
                    std::cout << "Reason: " << e.what() << std::endl;                    
                }
                if( plugin ){ // We succeeded on loading the plugin...
                    try{
                        Renderable* renderable = plugin->Create();
                        renderable->load( diter->path().native() );
                        plugin->Destroy( renderable );
                    }
                    catch( std::runtime_error e ){
                        std::cout << "Failed to load " << diter->path().filename() << std::endl;
                        std::cout << "Reason: " << e.what() << std::endl;                    
                    }
                }
            }
        }
    }


}
