//#####################################################################
// Copyright (c) 2016, Mridul Aanjaneya.
//#####################################################################
// Class NovaMesh_Model
//##################################################################### 
#ifndef __NOVAMESH_MODEL_H__
#define __NOVAMESH_MODEL_H__
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "NovaMesh_Mesh.h"

namespace Nova{
class NovaMesh_Model
{
    // model data
    std::vector<NovaMesh_Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;           // stores all the textures loaded so far
    
  public:
    NovaMesh_Model();
    ~NovaMesh_Model() {}

//##################################################################### 
    void Draw(const Shader& shader);
    void Load_Model(const std::string& path);
    void Process_Node(aiNode *node,const aiScene *scene);
    NovaMesh_Mesh Process_Mesh(aiMesh *mesh,const aiScene *scene);
    void Load_Material_Textures(aiMaterial *material,aiTextureType type,const std::string& type_name,std::vector<Texture>& textures);
//##################################################################### 
};
}
#endif
