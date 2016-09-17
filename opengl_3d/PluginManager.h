#ifndef __OPENGL_3D_PLUGIN_MANAGER_H__
#define __OPENGL_3D_PLUGIN_MANAGER_H__

#include "Plugin.h"
#include <map>
#include <vector>
#include <string>
#include <memory>

namespace Nova {

    class ApplicationFactory;

    class PluginManager {
    public:
        PluginManager(ApplicationFactory& app);

	void ClearSearchPaths();
	void AppendSearchPaths( std::vector<std::string> paths );
        void SetSearchPaths( std::vector<std::string> paths );
        void LoadPlugin(std::string name);

    private:
        ApplicationFactory& _app;
        std::vector<std::string> _search_paths;
        std::map< std::string, std::unique_ptr<Plugin> > _plugins;
        
    };

}

#endif
