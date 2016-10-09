#include "ViewportManager.h"
#include "ApplicationFactory.h"
#include "IOEvent.h"
#include "Camera.h"
#include "CameraControlInterface.h"

#include <stdexcept>

Nova::ViewportManager::ViewportManager( ApplicationFactory &app ) :
    _app(app), _geometry(0,0), _activeViewport(0), _recentlyInteractedViewport(0) {

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
    }
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
            viewport.camera->Set_Clipping(viewport.clipNear, viewport.clipFar);
            _viewports.push_back( std::move(viewport) );
        }
        break;
    default:
        throw std::logic_error("ViewportManager was given a configuration that was unknown." );
    };

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
    viewport.camera->Update();
}

void
Nova::ViewportManager::Set_ClipBounds( float near, float far ){
    // Update active viewport's clip bounds
    Viewport& viewport = _viewports.at( _activeViewport );
    viewport.clipNear = near;
    viewport.clipFar = far;
    viewport.camera->Set_Clipping(viewport.clipNear, viewport.clipFar);
}

void
Nova::ViewportManager::Set_Controller( std::string controller ){
    // Replace the active viewport's controller with one from factory 'controller'
    auto res = _controllerFactories.find( controller );
    if( res == _controllerFactories.end() )
        throw std::runtime_error( "Cannot replace viewport controller. No such controller type is currently registered");

    Viewport& viewport = _viewports.at( _activeViewport );
    viewport.controller.reset();
    viewport.controller = res->second->BuildCameraController( *(viewport.camera.get()) ); // Is this the best way???
    viewport.controller->Reset();
}

void 
Nova::ViewportManager::Set_Ortho(bool ortho){
    // Switch the projection mode of the currently active viewport
    Viewport& viewport = _viewports.at( _activeViewport );
    if( ortho )
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
    }

    if( event.type == IOEvent::MOUSEMOVE ){
        // Here we will try to find the currently interacting viewport if we are not currently focus locked.
        if( !_focused ){
            for( int v = 0; v < this->_viewports.size(); v++ ){
                glm::vec2 m = glm::vec2( event.mousemotion_data->x, event.mousemotion_data->y );
                if( m.x >= _viewports[v].x && m.x <= _viewports[v].x+_viewports[v].width &&
                    m.y >= _viewports[v].y && m.y <= _viewports[v].y+_viewports[v].height ){
                    _recentlyInteractedViewport = v;
                    break;
                }
            }
        }
        else{
            // Otherwise, we dispatch the event to the focused window
            if(_viewports.at( _focusedViewport ).controller)
                _viewports.at( _focusedViewport ).controller->MouseMove( event );
        }
    }




}


