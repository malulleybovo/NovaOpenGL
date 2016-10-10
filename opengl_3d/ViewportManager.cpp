#include "ViewportManager.h"
#include "ApplicationFactory.h"
#include "IOEvent.h"
#include "Camera.h"
#include "CameraControlInterface.h"

#include "StaticCameraControl.h"
#include "TrackballCameraControl.h"
#include "OrbitCameraControl.h"

#include <stdexcept>

#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/constants.hpp>

Nova::ViewportManager::ViewportManager( ApplicationFactory &app ) :
    _app(app), _geometry(0,0), _activeViewport(0), _recentlyInteractedViewport(0), axis_mode(AXIS_CORNER) {

    // Set up some built in control schemes
    _controllerFactories.insert( std::make_pair( "STATIC-CAMERA", std::move( std::unique_ptr<CameraFactory>( new SimpleCameraFactory<StaticCameraControl> ))));
    _controllerFactories.insert( std::make_pair( "ORBIT-CAMERA", std::move( std::unique_ptr<CameraFactory>( new SimpleCameraFactory<OrbitCameraControl> ))));
    _controllerFactories.insert( std::make_pair( "TRACKBALL-CAMERA", std::move( std::unique_ptr<CameraFactory>( new SimpleCameraFactory<TrackballCameraControl> ))));
    
    this->ConfigureViewports( VM_SINGLE_VIEWPORT );
}

Nova::ViewportManager::~ViewportManager() {

}

unsigned int
Nova::ViewportManager::NumViewports(){
    return _viewports.size();
}


unsigned int
Nova::ViewportManager::CurrentViewport() {
    return _activeViewport;
}


void
Nova::ViewportManager::SetWindowGeometry(int width, int height) {
    // Update all viewports with the new global window geometry
    _geometry = glm::vec2( width, height );
   
    // Now update all viewports based on the active configuration
    switch( _currentConfiguration ){
    case VM_SINGLE_VIEWPORT:
        {
            Viewport& viewport = _viewports[0]; // This mode only has one viewport
            // ... and its pretty easy to do so.
            viewport.x = 0;
            viewport.y = 0;
            viewport.width  = _geometry.x;
            viewport.height = _geometry.y;
           
        }
        break;
    case VM_DUAL_VIEWPORT:
        {
            Viewport& viewportA = _viewports[0]; 
            viewportA.x = 0;
            viewportA.y = 0;
            viewportA.width  = _geometry.x/2;
            viewportA.height = _geometry.y;

            Viewport& viewportB = _viewports[1]; 
            viewportB.x = _geometry.x/2;
            viewportB.y = 0;
            viewportB.width  = _geometry.x/2;
            viewportB.height = _geometry.y;

        }
        break;
    }

    for( int v = 0; v < this->_viewports.size(); v++ )
        // Next, we need to manually update each controller based on its viewport's
        // unique geometry
        UpdateViewportGeometry(v);   
}

void
Nova::ViewportManager::UpdateViewportGeometry( unsigned int v ){
    Viewport& viewport = _viewports.at(v);
    Nova::IOEvent event(IOEvent::DRAW);
    event.draw_data->x = viewport.x;
    event.draw_data->y = viewport.y;
    event.draw_data->width = viewport.width;
    event.draw_data->height = viewport.height;
    if( viewport.controller )
        viewport.controller->Redraw( event );                
}
    
void
Nova::ViewportManager::SetViewport( unsigned int viewport ){
    if( viewport >= _viewports.size() )
        throw std::out_of_range( "Unable to switch active viewport. Reqested viewport out of range." );
    _activeViewport = viewport;
}


void 
Nova::ViewportManager::ConfigureViewports( ViewportConfiguration vc ){
    // Setup viewports based on the specific configuration layout.

    _viewports.clear(); // We can do this safely as the unique_ptr's will clean up the camera and controller
    _currentConfiguration = vc;

    switch( vc ){
    case VM_SINGLE_VIEWPORT:
        { // Create a single viewport that fills the entire window geometry
            
            Viewport viewport;
            viewport.x = 0;
            viewport.y = 0;
            viewport.width  = _geometry.x;
            viewport.height = _geometry.y;
            viewport.clipNear = 0.1f;
            viewport.clipFar = 100.0f;
            viewport.camera = std::unique_ptr<Camera>( new Camera() );
            viewport.camera->Set_Mode(FREE);
            viewport.camera->Set_FOV(45);
            viewport.camera->Set_Position(glm::vec3(0,0,-10));
            viewport.camera->Set_Look_At(glm::vec3(0,0,0));
            _viewports.push_back( std::move(viewport) );

            Set_Controller("ORBIT-CAMERA", 0);
        }
        break;
    case VM_DUAL_VIEWPORT:
        { // Create two horizontal viewports that fill the entire window geometry
            {
                Viewport viewport;
                viewport.x = 0;
                viewport.y = 0;
                viewport.width  = _geometry.x/2;
                viewport.height = _geometry.y;
                viewport.clipNear = 0.1f;
                viewport.clipFar = 100.0f;
                viewport.camera = std::unique_ptr<Camera>( new Camera() );
                viewport.camera->Set_Mode(FREE);
                viewport.camera->Set_FOV(45);
                viewport.camera->Set_Position(glm::vec3(0,0,-10));
                viewport.camera->Set_Look_At(glm::vec3(0,0,0));
                _viewports.push_back( std::move(viewport) );
            }
            {
                Viewport viewport;
                viewport.x = _geometry.x/2;
                viewport.y = 0;
                viewport.width  = _geometry.x/2;
                viewport.height = _geometry.y;
                viewport.clipNear = 0.1f;
                viewport.clipFar = 100.0f;
                viewport.camera = std::unique_ptr<Camera>( new Camera() );
                viewport.camera->Set_Mode(FREE);
                viewport.camera->Set_FOV(45);
                viewport.camera->Set_Position(glm::vec3(0,0,-10));
                viewport.camera->Set_Look_At(glm::vec3(0,0,0));
                _viewports.push_back( std::move(viewport) );
            }
            Set_Controller("ORBIT-CAMERA", 0);
            Set_Controller("ORBIT-CAMERA", 1);
        }
        break;
    default:
        throw std::logic_error("ViewportManager was given a configuration that was unknown." );
    };
    _activeViewport = 0;
    _recentlyInteractedViewport = 0;
    _focusedViewport = 0;
}

void
Nova::ViewportManager::InitializeChrome(){
    
    // Initialize buffers for the chrome
    glGenVertexArrays(1, &this->chromeVAO);
    glGenBuffers(1, &this->chromeVBO);
    glBindVertexArray(this->chromeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->chromeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Initialize buffers for the axis displays
    glGenVertexArrays(1, &this->axisVAO);
    glGenBuffers(1, &this->axisVBO);
    glBindBuffer(GL_ARRAY_BUFFER, this->axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void
Nova::ViewportManager::DrawFrame(){
    // Perhaps there is a way to dynamically determine the correct
    // dividing lines between panes, but for now we will hardcode them
    glViewport(0,0,_geometry.x,_geometry.y);
    
    auto _shader = _app.GetShaderManager().GetShader( "ViewportChromeShader" );
    _shader->SetMatrix4("projection", glm::ortho(0.0f,
                                                 static_cast<GLfloat>(_geometry.x),
                                                 static_cast<GLfloat>(_geometry.y),
                                                 0.0f));
    _shader->SetInteger("panes", _viewports.size() );
    _shader->SetFloat("thickness", 2.0f ); // Two pixels thick
    _shader->SetFloat("width", _geometry.x );
    _shader->SetFloat("height", _geometry.y );
    glBindVertexArray(this->chromeVAO);
    GLfloat vertices[6][4] = {
        { 0,     _geometry.y,   0.0, 1.0 },
        { _geometry.x, 0,       1.0, 0.0 },
        { 0,     0,       0.0, 0.0 },

        { 0,     _geometry.y,   0.0, 1.0 },
        { _geometry.x, _geometry.y,   1.0, 1.0 },
        { _geometry.x, 0,       1.0, 0.0 }
    };
    glBindBuffer(GL_ARRAY_BUFFER, this->chromeVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);    
    glBindVertexArray(0);    
};


void Nova::ViewportManager::DrawAxis(){
    if( axis_mode == AXIS_DISABLE )
        return;
    
    Viewport& viewport = _viewports.at( _activeViewport );

    auto _shader = _app.GetShaderManager().GetShader( "ViewportAxisShader" );
    glm::mat4 projection,view,model;
    model = viewport.camera->Get_ModelMatrix();
    view = viewport.camera->Get_ViewMatrix();
    glm::mat3 rot_part( view );   
    glm::mat4 mReflectionX = glm::mat4
        (
         -1.0, 0.0, 0.0, 0.0,
         0.0, 1.0, 0.0, 0.0,
         0.0, 0.0, 1.0, 0.0,
         0.0, 0.0, 0.0, 1.0
         );
    
    view =  glm::rotate( mReflectionX * glm::mat4(rot_part), glm::pi<float>(), glm::vec3( 1.0, 0, 0) ) * mReflectionX;
    if( axis_mode == AXIS_CENTER ){
        projection = glm::ortho(-1.0f * ((float)(viewport.width) / viewport.height),
                                  1.0f * ((float)(viewport.width) / viewport.height), 
                                  1.0f ,
                                  -1.0f,
                                  -1.0f,
                                  1.0f);        
        glViewport( viewport.x, viewport.y, viewport.width, viewport.height );
    }
    if( axis_mode == AXIS_CORNER ){
        projection = glm::ortho(-1.0f,
                                  1.0f, 
                                  1.0f ,
                                  -1.0f,
                                  -1.0f,
                                  1.0f);        
        glViewport( viewport.x, viewport.y, 50, 50 );
    }
    
    _shader->SetMatrix4("projection",projection);
    _shader->SetMatrix4("view",view);
    _shader->SetMatrix4("model",model);
    
    glBindVertexArray(this->axisVAO);
    
    GLfloat vertices[1][4] = {
        { 0,  0,  0, 1.0 }
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, this->axisVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glDrawArrays(GL_POINTS, 0, 1);
    glBindVertexArray(0);  
}

std::vector< std::string >
Nova::ViewportManager::AvailableControllers(){
    // TODO: return all the registered controller factories by name

    return std::vector<std::string>();
}

void
Nova::ViewportManager::ResetAll(){
    // Reset all camera controllers for all viewports
    for( auto&& viewport : _viewports )
        if( viewport.controller )
            viewport.controller->Reset();
}

void
Nova::ViewportManager::Update(){
    //  Run the update routine for the currently active viewport's camera controller
    Viewport& viewport = _viewports.at( _activeViewport );
    viewport.camera->Set_Clipping( viewport.clipNear, viewport.clipFar);
    viewport.camera->Set_Viewport( viewport.x, viewport.y, viewport.width, viewport.height );
    viewport.camera->Update();
}

void
Nova::ViewportManager::Set_ClipBounds( float near, float far, unsigned int v ){
    // Update active viewport's clip bounds
    Viewport& viewport = _viewports.at( v );
    viewport.clipNear = near;
    viewport.clipFar = far;    
}

void
Nova::ViewportManager::Set_Controller( std::string controller, unsigned int v ){
    // Replace the active viewport's controller with one from factory 'controller'
    auto res = _controllerFactories.find( controller );
    if( res == _controllerFactories.end() )
        throw std::runtime_error( "Cannot replace viewport controller. No such controller type is currently registered");

    Viewport& viewport = _viewports.at( v );
    if( viewport.controller )
        viewport.controller->Reset();
    viewport.controller.reset();
    viewport.controller = res->second->BuildCameraController( *(viewport.camera.get()) ); // Is this the best way???
    UpdateViewportGeometry(v);
    viewport.controller->Reset();
}

void 
Nova::ViewportManager::Set_Ortho(bool ortho, unsigned int v){
    // Switch the projection mode of the currently active viewport
    Viewport& viewport = _viewports.at( v );
    if( ortho )
        viewport.camera->Set_Mode( ORTHO );
    else
        viewport.camera->Set_Mode( FREE );
}

void 
Nova::ViewportManager::Toggle_Ortho(unsigned int v){
    // Switch the projection mode of the currently active viewport
    Viewport& viewport = _viewports.at( v );
    if( viewport.camera->Get_Mode() == FREE )
        viewport.camera->Set_Mode( ORTHO );
    else
        viewport.camera->Set_Mode( FREE );
}

glm::mat4
Nova::ViewportManager::GetViewMatrix() const {
    // Retrieve the view matrix from the active viewport
    const Viewport& viewport = _viewports.at( _activeViewport );
    return viewport.camera->Get_ViewMatrix();
}

glm::mat4
Nova::ViewportManager::GetModelMatrix() const{
    // Retrieve the model matrix from the active viewport
    const Viewport& viewport = _viewports.at( _activeViewport );
    return viewport.camera->Get_ModelMatrix();

}

glm::mat4
Nova::ViewportManager::GetProjectionMatrix() const{
    // Retrieve the projection matrix from the active viewport
    const Viewport& viewport = _viewports.at( _activeViewport );
    return viewport.camera->Get_ProjectionMatrix();

}

glm::vec4
Nova::ViewportManager::GetViewport() const{
    // Retrieve the viewport parameters from the active viewport
    const Viewport& viewport = _viewports.at( _activeViewport );
    return glm::vec4( viewport.x, viewport.y, viewport.width, viewport.height );
}


glm::mat4
Nova::ViewportManager::GetInteractionViewMatrix() const {
    if( _focused ){
        const Viewport& viewport = _viewports.at( _focusedViewport );
        return viewport.camera->Get_ViewMatrix();
    }
    else{
        const Viewport& viewport = _viewports.at( _recentlyInteractedViewport );
        return viewport.camera->Get_ViewMatrix();
    }       
}

glm::mat4
Nova::ViewportManager::GetInteractionModelMatrix() const {
    if( _focused ){
        const Viewport& viewport = _viewports.at( _focusedViewport );
        return viewport.camera->Get_ModelMatrix();
    }
    else{
        const Viewport& viewport = _viewports.at( _recentlyInteractedViewport );
        return viewport.camera->Get_ModelMatrix();
    }   
}

glm::mat4
Nova::ViewportManager::GetInteractionProjectionMatrix() const {
    if( _focused ){
        const Viewport& viewport = _viewports.at( _focusedViewport );
        return viewport.camera->Get_ProjectionMatrix();
    }
    else{
        const Viewport& viewport = _viewports.at( _recentlyInteractedViewport );
        return viewport.camera->Get_ProjectionMatrix();
    }  
}

glm::vec4
Nova::ViewportManager::GetInteractionViewport() const {
    if( _focused ){
        const Viewport& viewport = _viewports.at( _focusedViewport );
        return glm::vec4( viewport.x, viewport.y, viewport.width, viewport.height );
    }
    else{
        const Viewport& viewport = _viewports.at( _recentlyInteractedViewport );
        return glm::vec4( viewport.x, viewport.y, viewport.width, viewport.height );
    }  
}

void
Nova::ViewportManager::HandleEvent( Nova::IOEvent& event ){
    // The viewport manager's event dispatching call
    const Viewport& viewport = _viewports.at( _activeViewport );
    const Viewport& interacted_viewport = _viewports.at( _recentlyInteractedViewport );

    if( event.type == IOEvent::COMMAND ){
        if( event.command_data->command == "RESET-CAMERA" ){
            if(interacted_viewport.controller)
                interacted_viewport.controller->Reset();
        }
        if( event.command_data->command == "WINDOW-RESIZE" ){
            if( event.data_flags | IOEvent::DRAW ){
                this->SetWindowGeometry( event.draw_data->width, event.draw_data->height );
            }
        }
        if( event.command_data->command == "RENDER-FRAME" ){
            for( auto&& v : _viewports )
                if( v.controller )
                    v.controller->Update( event );
        }
        if( event.command_data->command == "SWITCH-CAMERA" ){
            if( event.command_data->args.size() < 1 ){
                std::cout << "Usage: SWITCH-CAMERA <CameraType>" << std::endl;                
            }
            else{                
                // We don't need to disable focus, as the new controller will tell us if we regain it.
                // Remember, by default focus is lost quickly unless it is refreshed every input event.
                // However, if we are focused, the camera controller of the focused viewport is replaced.
                if( _focused ){
                    try{                        
                        Set_Controller( event.command_data->args.at(0), _focusedViewport );
                    }
                    catch( std::exception& e ){
                        std::cout << e.what() << std::endl;
                    }
                }
                // Otherwise, we try to replace the last interacted viewport...
                else{
                    try{                        
                        Set_Controller( event.command_data->args.at(0), _recentlyInteractedViewport );
                    }
                    catch( std::exception& e ){
                        std::cout << e.what() << std::endl;
                    }                    
                }                
            }
        }
        if( event.command_data->command == "VIEWPORT-MODE" ){
            if( event.command_data->args.size() < 1 ){
                std::cout << "Usage: VIEWPORT-MODE <Mode>" << std::endl;                
            }
            if( event.command_data->args[0] == "SINGLE" && _viewports.size() != 1 )
                ConfigureViewports( VM_SINGLE_VIEWPORT );
            if( event.command_data->args[0] == "DUAL" && _viewports.size() != 2 )
                ConfigureViewports( VM_DUAL_VIEWPORT );
            //if( event.command_data->args[0] == "QUAD" && _viewports.size() != 4 )
            //    ConfigureViewports( VM_QUAD_VIEWPORT );
        }

        if( event.command_data->command == "CYCLE-AXIS-MODE" ){
            axis_mode = (axis_mode + 1) % AXIS_MAX_VALUE;
        }

        if( event.command_data->command == "TOGGLE-ORTHO-CAMERA" ){
            if( _focused )
                Toggle_Ortho( _focusedViewport );
            else
                Toggle_Ortho( _recentlyInteractedViewport );
        }
    }

    if( event.type == IOEvent::MOUSEMOVE ){
        // Here we will try to find the currently interacting viewport if we are not currently focus locked.
        if( !_focused ){
            // Determine which viewport we are currently interacting with...
            for( int v = 0; v < this->_viewports.size(); v++ ){
                glm::vec2 m = glm::vec2( event.mousemotion_data->x, event.mousemotion_data->y );
                if( m.x >= _viewports[v].x && m.x <= _viewports[v].x+_viewports[v].width &&
                    m.y >= _viewports[v].y && m.y <= _viewports[v].y+_viewports[v].height ){                    
                    _recentlyInteractedViewport = v;
                    break;
                }
            }

            // Send event to the interacting viewport
            if( _viewports.at(_recentlyInteractedViewport).controller)
                _viewports.at(_recentlyInteractedViewport).controller->MouseMove(event);
        }
        else{
            // Otherwise, we dispatch the event to the focused window
            if(_viewports.at( _focusedViewport ).controller)
                _viewports.at( _focusedViewport ).controller->MouseMove( event );
        }
    }

    if( event.type == IOEvent::MOUSEBUTTON ){
        if( !_focused ){
            if( _viewports.at(_recentlyInteractedViewport).controller ){
                if( event.mousebutton_data->action == IOEvent::M_DOWN )
                    _viewports.at(_recentlyInteractedViewport).controller->MouseDown(event);
                if( event.mousebutton_data->action == IOEvent::M_UP )
                    _viewports.at(_recentlyInteractedViewport).controller->MouseUp(event);
            }                                
        }
        else{
            if( _viewports.at(_focusedViewport).controller ){
                if( event.mousebutton_data->action == IOEvent::M_DOWN )
                    _viewports.at(_focusedViewport).controller->MouseDown(event);
                if( event.mousebutton_data->action == IOEvent::M_UP )
                    _viewports.at(_focusedViewport).controller->MouseUp(event);
            }                                
        }
    }

    if( event.type == IOEvent::KEYBOARD ){
        if( !_focused ){
            if( _viewports.at(_recentlyInteractedViewport).controller ){
                if( event.key_data->action == IOEvent::K_DOWN )
                    _viewports.at(_recentlyInteractedViewport).controller->KeyDown(event);
                if( event.key_data->action == IOEvent::K_UP )
                    _viewports.at(_recentlyInteractedViewport).controller->KeyUp(event);
                if( event.key_data->action == IOEvent::K_HOLD )
                    _viewports.at(_recentlyInteractedViewport).controller->KeyHold(event);
            }                                
        }
        else{
            if( _viewports.at(_focusedViewport).controller ){
                if( event.key_data->action == IOEvent::K_DOWN )
                    _viewports.at(_focusedViewport).controller->KeyDown(event);
                if( event.key_data->action == IOEvent::K_UP )
                    _viewports.at(_focusedViewport).controller->KeyUp(event);
                if( event.key_data->action == IOEvent::K_HOLD )
                    _viewports.at(_focusedViewport).controller->KeyHold(event);
            }                                
        }
    }
    
    if( event.type == IOEvent::SCROLL ){
        if( !_focused ){
            if( _viewports.at(_recentlyInteractedViewport).controller )
                _viewports.at(_recentlyInteractedViewport).controller->Scroll(event);
        }
        else{
            if( _viewports.at(_focusedViewport).controller )
                _viewports.at(_focusedViewport).controller->Scroll(event);
        }
    }

    
    // Check for viewport focus. Assume nothing is focused unless we are told otherwise
    bool _prevFocus = _focused;
    _focused = false;
    for( int v = 0; v < this->_viewports.size(); v++ ){
        if( _viewports.at( v ).controller)
            if( _viewports.at( v ).controller->HasFocus()){
                _focused = true;
                _focusedViewport = v;
            }                
    }
}


