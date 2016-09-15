//#####################################################################
// Copyright (c) 2016, Mridul Aanjaneya.
//#####################################################################
#include "NovaMesh_Model.h"
#include <nova/Rendering/OpenGL/Utilities/OpenGL_Utilities.h>
#include <assimp/postprocess.h>
#include <iostream>
using namespace Nova;
//#####################################################################
// Constructor
//#####################################################################
NovaMesh_Model::
NovaMesh_Model()
{
}
//#####################################################################
// Draw
//#####################################################################
void NovaMesh_Model::
Draw(const Shader& shader)
{
    shader.Use();
    for(GLuint i=0;i<meshes.size();++i) meshes[i].Draw(shader);
}
//#####################################################################
// Load_Model
//#####################################################################
void NovaMesh_Model::
Load_Model(const std::string& path)
{
    std::cout << "NovaMesh_Model::Load_Model" << std::endl;
            
    Assimp::Importer importer;
    const aiScene* scene=importer.ReadFile(path,aiProcessPreset_TargetRealtime_MaxQuality);

    if(!scene || scene->mFlags==AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout<<"ERROR::ASSIMP:: "<<importer.GetErrorString()<<std::endl;
        return;
    }

    std::cout << "Model contains: " << scene->mRootNode->mNumMeshes << " meshes;" << std::endl;

    directory=path.substr(0,path.find_last_of('/'));

    // process ASSIMP's root node recursively
    Process_Node(scene->mRootNode,scene);
}
//#####################################################################
// Process_Node
//#####################################################################
void NovaMesh_Model::
Process_Node(aiNode* node,const aiScene* scene)
{
    // process each mesh located at the current node
    for(GLuint i=0;i<node->mNumMeshes;++i)
    {        
        aiMesh *mesh=scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(Process_Mesh(mesh,scene));
    }

    // recursively process each of the children nodes
    for(GLuint i=0;i<node->mNumChildren;++i)
        Process_Node(node->mChildren[i],scene);
}
//#####################################################################
// Process_Mesh
//#####################################################################
NovaMesh_Mesh NovaMesh_Model::
Process_Mesh(aiMesh *mesh,const aiScene *scene)
{
    std::cout << "Processing mesh" << std::endl;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    for(GLuint i=0;i<mesh->mNumVertices;++i)
    {
        Vertex vertex;
        glm::vec3 vector;

        // positions
        vector.x=mesh->mVertices[i].x;
        vector.y=mesh->mVertices[i].y;
        vector.z=mesh->mVertices[i].z;
        vertex.position=vector;

        // normals
        vector.x=mesh->mNormals[i].x;
        vector.y=mesh->mNormals[i].y;
        vector.z=mesh->mNormals[i].z;
        vertex.normal=vector;

        // texture coordinates
        if(mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x=mesh->mTextureCoords[0][i].x;
            vec.y=mesh->mTextureCoords[0][i].y;
            vertex.tex_coords=vec;
        }
        else vertex.tex_coords=glm::vec2(0.0f,0.0f);
        
        vertices.push_back(vertex);
    }

    // walk through all the faces
    for(GLuint i=0;i<mesh->mNumFaces;++i)
    {
        aiFace face=mesh->mFaces[i];
        for(GLuint j=0;j<face.mNumIndices;++j) indices.push_back(face.mIndices[j]);
    }

    // process materials
    if(mesh->mMaterialIndex>=0)
    {
        aiMaterial *material=scene->mMaterials[mesh->mMaterialIndex];

        // diffuse maps
        std::vector<Texture> diffuse_maps;
        Load_Material_Textures(material,aiTextureType_DIFFUSE,"texture_diffuse",diffuse_maps);
        textures.insert(textures.end(),diffuse_maps.begin(),diffuse_maps.end());
        // specular maps
        std::vector<Texture> specular_maps;
        Load_Material_Textures(material,aiTextureType_SPECULAR,"texture_specular",specular_maps);
        textures.insert(textures.end(),specular_maps.begin(),specular_maps.end());
    }

    return NovaMesh_Mesh(vertices,indices,textures);
}
//#####################################################################
// Load_Material_Textures
//#####################################################################
void NovaMesh_Model::
Load_Material_Textures(aiMaterial *material,aiTextureType type,const std::string& type_name,std::vector<Texture>& textures)
{
    for(GLuint i=0;i<material->GetTextureCount(type);++i)
    {
        aiString str;
        material->GetTexture(type,i,&str);
        // skip texture if it was loaded before
        GLboolean skip=false;
        for(GLuint j=0;j<textures_loaded.size();++j)
        {
            if(textures_loaded[j].path==str)
            {
                textures.push_back(textures_loaded[j]);
                skip=true;break;
            }
        }

        if(!skip)
        {
            Texture texture;
            texture.id=Texture_From_File(str.C_Str(),directory);
            texture.type=type_name;
            texture.path=str;
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
}
//#####################################################################
