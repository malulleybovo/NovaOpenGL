#include "PluginManager.h"
#include <exception>
#include <iostream>
#include <utility>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

Nova::PluginManager::PluginManager(ApplicationFactory& app) : _app(app)
{

}

void 
Nova::PluginManager::SetSearchPaths(std::vector<std::string> paths)
{
    _search_paths = paths;
}

Nova::Plugin*
Nova::PluginManager::Get( std::string name ){
    try{
        return _plugins.at( name ).get();
    }
    catch( std::out_of_range& e ){
        std::cout << "Requested plugin '"<< name << "' not loaded. Searching for plugin." << std::endl;
        
        for( auto p : _search_paths ){
            fs::path potential_path;
            potential_path /= p;
            potential_path /= ( name + std::string(".so") );
            
            std::cout << "Trying to load '" << potential_path.native() << "'"<< std::endl;
            if( fs::is_symlink( potential_path ) )
                potential_path = fs::read_symlink( potential_path );
            
            if( fs::exists( potential_path ) ){
                potential_path = fs::canonical( potential_path );
                bool loadSuccessful = false;
                try{
                    std::unique_ptr<Plugin> plugin( new Plugin( name, potential_path.native() ) );
                    _plugins.insert( std::make_pair(name, std::move(plugin) ));
                    loadSuccessful = true;
                }
                catch( int e ){
                    
                }
                if( loadSuccessful ){
                    std::cout << "Plugin Loaded." << std::endl;
                    break;
                }
            }
            else{
                throw std::runtime_error(std::string("Unable to load plugin '") + name + std::string("'"));
            }
        }

        return _plugins.at(name).get();
    }
}
