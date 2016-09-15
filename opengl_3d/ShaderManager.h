#ifndef __OPENGL_3D_SHADER_MANAGER_H__
#define __OPENGL_3D_SHADER_MANAGER_H__

#include "Shader.h"
#include <map>

namespace Nova {

    class ApplicationFactory;

    class ShaderManager{
    public:
        ShaderManager(ApplicationFactory& app);

        const Shader* GetBuiltinShader(const std::string& name);
        const Shader* GetShader(const std::string& name);
        
    private:
        ApplicationFactory& _app;
        std::map<std::string, Shader> _builtin_shaderRepo;
        std::map<std::string, Shader> _shaderRepo;
    };
}

#endif
