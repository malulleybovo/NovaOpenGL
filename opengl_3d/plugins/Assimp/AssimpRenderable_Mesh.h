//#####################################################################
// Copyright (c) 2016, Mridul Aanjaneya.
//#####################################################################
// Class Mesh
//##################################################################### 
#ifndef __ASSIMP_RENDERABLE_MESH_H__
#define __ASSIMP_RENDERABLE_MESH_H__

#include "../../Shader.h"
#include <assimp/types.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace Nova{
class Vertex
{
  public:
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
};

class Texture
{
  public:
    GLuint id;
    std::string type;
    aiString path;
};

class AssimpRenderable_Mesh
{
    GLuint VAO,VBO,EBO;                 // render data
  public:
    std::vector<Vertex> vertices;       // mesh data
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    AssimpRenderable_Mesh(const std::vector<Vertex>& vertices_input,const std::vector<GLuint>& indices_input,const std::vector<Texture>& textures_input);
    ~AssimpRenderable_Mesh() {}

//##################################################################### 
    void Setup_Mesh();
    void Draw(const Shader& shader);
//##################################################################### 
};
}
#endif
