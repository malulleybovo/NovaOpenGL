//#####################################################################
// Copyright (c) 2016, Mridul Aanjaneya.
//#####################################################################
// Class AssimpRenderable_Model
//##################################################################### 
#ifndef __ASSIMP_RENDERABLE_MODEL_H__
#define __ASSIMP_RENDERABLE_MODEL_H__
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "AssimpRenderable_Mesh.h"

namespace Nova{
class AssimpRenderable_Model
{
    // model data
    std::vector<AssimpRenderable_Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;           // stores all the textures loaded so far
    
  public:
    AssimpRenderable_Model();
    ~AssimpRenderable_Model() {}

//##################################################################### 
    void Draw(const Shader& shader);
    void Load_Model(const std::string& path);
    void Process_Node(aiNode *node,const aiScene *scene);
    AssimpRenderable_Mesh Process_Mesh(aiMesh *mesh,const aiScene *scene);
    void Load_Material_Textures(aiMaterial *material,aiTextureType type,const std::string& type_name,std::vector<Texture>& textures);
//##################################################################### 
};
}
#endif
