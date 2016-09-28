#ifndef __OPENGL_3D_CONFIG_H__
#define __OPENGL_3D_CONFIG_H__

#include <string>
#include <vector>

namespace Nova {

    class Config {
    public:
        Config();
        void Parse(const std::vector<std::string>& paths, int argc, char** argv);
    
        std::string scenepath;
        std::vector<std::string> pluginpaths;
        std::vector<std::string> bindings;
        std::string fontname;
    };
}


#endif
