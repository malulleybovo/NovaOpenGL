#include "ShaderManager.h"

// Include all Builtin Shaders //
#include "shaders/BasicMeshShader.h"


// Done                        //


Nova::ShaderManager::ShaderManager()
{
    // Need to populate the Built-in Shader maps. 
    // These shaders are compiled in and never change

    

}

const Nova::Shader*
Nova::ShaderManager::GetBuiltinShader(const std::string& name)
{

    return nullptr;
}

const Nova::Shader*
Nova::ShaderManager::GetShader(const std::string& name)
{

    return nullptr;
}
