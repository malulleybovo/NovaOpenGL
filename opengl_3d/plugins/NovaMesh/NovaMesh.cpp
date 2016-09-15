#include "../../Renderable.h"
#include "../../ApplicationFactory.h"
#include "../../Shader.h"

#include "NovaMesh_Model.h"
#include "../../shaders/BasicMeshShader.h"

#include <iostream>
#include <fstream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Nova {
        
    class NovaMesh : public Renderable {        
        std::unique_ptr<NovaMesh_Model> _model;
        std::unique_ptr<Shader> _shader;

    public:
        NovaMesh( ApplicationFactory& app ) : Renderable( app )
        {
            _model = std::unique_ptr<NovaMesh_Model>( new NovaMesh_Model() );
            _shader = std::unique_ptr<Shader>( new Shader() );
            _shader->LoadFromString(NovaBuiltinShaders::BasicMeshShader::vertex_shader,
                                    NovaBuiltinShaders::BasicMeshShader::fragment_shader);
        }

        virtual ~NovaMesh(){
            
        }
        
        virtual void load(std::string path){
            std::cout << "NovaMesh::load" << std::endl;            
            _model->Load_Model( path );
        }


        virtual void draw(){
            glm::mat4 projection,view,model;
            view = _app.GetWorld().Get_ViewMatrix();
            model = _app.GetWorld().Get_ModelMatrix();
            projection = _app.GetWorld().Get_ProjectionMatrix();
            
            GLuint projection_location=glGetUniformLocation(_shader->GetProgramId(),"projection");
            glUniformMatrix4fv(projection_location,1,GL_FALSE,glm::value_ptr(projection));
            GLuint view_location=glGetUniformLocation(_shader->GetProgramId(),"view");
            glUniformMatrix4fv(view_location,1,GL_FALSE,glm::value_ptr(view));
            GLuint model_location=glGetUniformLocation(_shader->GetProgramId(),"model");
            glUniformMatrix4fv(model_location,1,GL_FALSE,glm::value_ptr(model));
            _model->Draw(*(_shader.get()));
        }
        

    };

}

extern "C" Nova::Renderable* Nova_CreateRenderable(Nova::ApplicationFactory& app) {
    return new Nova::NovaMesh(app);
}

extern "C" void Nova_DestroyRenderable( Nova::Renderable* r) {
    delete r;
}
