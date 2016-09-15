//#####################################################################
// Copyright (c) 2016, Mridul Aanjaneya.
//#####################################################################
// Class Shader
//##################################################################### 
#ifndef __OPENGL_3D_SHADER_H__
#define __OPENGL_3D_SHADER_H__

#include <GL/glew.h>
#include <string>

namespace Nova{
class Shader
{
private:
    GLuint program;         // the shader program id
    
public:
    
    Shader();
    ~Shader();
    void LoadFromFiles(std::string vertex_path,std::string fragment_path);
    void LoadFromString(const GLchar* vertex_shader,const GLchar* fragment_shader);
    GLuint GetProgramId() const ;

//##################################################################### 
    void Use() const ;
//##################################################################### 
};
}
#endif
