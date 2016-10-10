#include "Scene.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include <string>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <utility>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/intersect.hpp> 

#include "ApplicationFactory.h"
#include "IOEvent.h"

#include "shaders/BasicLineShader.h"

Nova::Scene::Scene(ApplicationFactory& app) : _app(app), _debugHitSelection(false)
{
    hit_start = glm::vec3(0,0,0);
    hit_end = glm::vec3(1,0,0);
    selected_object = _app.GetRenderableManager().MaxId();
    _app.GetIOService().On( "TOGGLE-DEBUG", [&](IOEvent& event){
            if( event.command_data->args.size() >= 1 &&
                event.command_data->args.at(0) == "SELECTION" )
                _debugHitSelection = !_debugHitSelection;
        });
    
}

std::vector< std::string > 
tokenize( std::string str ){
    typedef std::string::size_type size_type;
    std::vector< std::string > tokens;
    size_type pos = 0;
    while( pos != std::string::npos ){
        size_type next_token = str.find_first_of(".", pos);
        if( next_token != std::string::npos ){
            tokens.push_back( str.substr( pos, next_token-pos ) );
            pos = next_token+1;
        }
        else{
            tokens.push_back( str.substr( pos ) );
            pos = std::string::npos;
        }
    }                         
    return tokens;
}

void
Nova::Scene::Configure( std::string path ){

    _path = path;

    fs::path config_path;
    config_path /= path;
    config_path /= "scene.conf";

    po::options_description config("Configuration");
    config.add_options()
        ;
    
    std::ifstream ifs(config_path.c_str());
    if (!ifs)
        return;

    po::variables_map vm;
    auto results = parse_config_file(ifs, config, true);
    store( results, vm );
    notify(vm);        

    for( auto plugin : _app.GetConfig().pluginList ){
      _app.GetPluginManager().LoadPlugin( plugin );
    }    
}

void
Nova::Scene::Load()
{

    fs::path load_path;
    load_path /= _path;    
    //load_path = fs::canonical(load_path);
    //std::cout << "Loading " << load_path.native() << std::endl;
    load_path /= "./";

    fs::directory_iterator diter( load_path );
    for( ; diter != fs::directory_iterator(); diter++){
        unsigned long objectId = _app.GetRenderableManager().MaxId();
        try{
            objectId = _app.GetRenderableManager().RegisterRenderable( diter->path().native() );
        }
        catch( std::exception& e ){
            // We failed to load this particular file, say something about it...
            //std::cout << "Couldn't load " << diter->path().native() << ", Reason: " << std::endl;
            //std::cout << e.what() << std::endl;
        }

        if( objectId != _app.GetRenderableManager().MaxId() ){
            _scene_objects.push_back( objectId );
            std::cout << "Loaded " << diter->path().filename() << " with id " << objectId << std::endl;
        }
    }

}


void Nova::Scene::Draw()
{
    if( _debugHitSelection ){
        GLfloat vertices[6];
        vertices[0] = hit_start.x;
        vertices[1] = hit_start.y;
        vertices[2] = hit_start.z;
        vertices[3] = hit_end.x;
        vertices[4] = hit_end.y;
        vertices[5] = hit_end.z;
        
        GLuint VBO;
        GLuint VAO;
        glGenBuffers(1, &VBO); 
        glGenVertexArrays(1, &VAO);  
        
        glBindVertexArray(VAO);
        // 2. Copy our vertices array in a buffer for OpenGL to use
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
        // 3. Then set our vertex attributes pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);  
        
        glm::mat4 projection,view,model;
        view = _app.GetWorld().Get_ViewMatrix();
        model = _app.GetWorld().Get_ModelMatrix();
        projection = _app.GetWorld().Get_ProjectionMatrix();
        auto lineShader = _app.GetShaderManager().GetShader("BasicLineShader");
        lineShader->SetMatrix4("projection",projection);
        lineShader->SetMatrix4("view",view);
        lineShader->SetMatrix4("model",model);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);      
    }

    for( auto objectId : _scene_objects ){
        Renderable* renderable = _app.GetRenderableManager().Get( objectId );
        renderable->draw();
    }
}


void 
Nova::Scene::Selection( IOEvent& event )
{
    if( event.mousebutton_data->button != IOEvent::M_LEFT || !(event.mousebutton_data->mods & IOEvent::mSHIFT) )
        return;

    // Build Hit Test line

    glm::vec3 mouse_position;
    glm::mat4 view = _app.GetWorld().Get_ViewMatrix();
    glm::mat4 model = _app.GetWorld().Get_ModelMatrix();
    glm::mat4 projection = _app.GetWorld().Get_ProjectionMatrix();
    glm::vec4 viewport = _app.GetWorld().Get_Viewport();

    mouse_position.x = event.mousebutton_data->x;
    mouse_position.y = (viewport[3] - event.mousebutton_data->y); // Fix the Y direction

    mouse_position.z = 0.0;
    hit_start = glm::unProject( mouse_position, view*model, projection, viewport );
    mouse_position.z = 1.0;
    hit_end = glm::unProject( mouse_position, view*model, projection, viewport );

    //std::cout << "Mouse Point: " << glm::to_string(mouse_position) << std::endl;
    //std::cout << "ViewPort" << glm::to_string(viewport) << std::endl;
    //std::cout << "Hit line : " << glm::to_string(hit_start) << " " << glm::to_string(hit_end) << std::endl;

    if( selected_object != _app.GetRenderableManager().MaxId() )
        _app.GetRenderableManager().Get( selected_object ) -> unassign_selection();
    
    float min_distance = std::numeric_limits<float>::infinity();
    selected_object = _app.GetRenderableManager().MaxId();
    for( auto objectId : _scene_objects ){
        Renderable* renderable = _app.GetRenderableManager().Get( objectId );
        
        // If we are selectable, run the hit tests now!
        if( renderable->selectable() ){
            // First prune with line-sphere intersection test
            glm::vec4 sphere = renderable->bounding_sphere();
            glm::vec3 sphere_insec_p1, sphere_insec_p2;
            glm::vec3 sphere_insec_n1, sphere_insec_n2;
            bool potentially_intersects = glm::intersectLineSphere( hit_start,
                                                                    hit_end,
                                                                    glm::vec3(sphere.x,sphere.y,sphere.z),
                                                                    sphere[3],
                                                                    sphere_insec_p1,
                                                                    sphere_insec_n1,
                                                                    sphere_insec_p2,
                                                                    sphere_insec_n2);
            if( potentially_intersects ){
                // Now do the real intersection test which might be very expensive
                float distance = renderable->hit_test( hit_start, hit_end );
                if( distance < min_distance ){
                    min_distance = distance;
                    selected_object = objectId;
                }
            }
        }
    }
    if( selected_object != _app.GetRenderableManager().MaxId() ){
        _app.GetRenderableManager().Get( selected_object ) -> assign_selection( hit_start,
                                                                                hit_end,
                                                                                glm::normalize( hit_end-hit_start )*min_distance + hit_start );       
    }

}
