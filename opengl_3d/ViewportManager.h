#ifndef __OPENGL_3D_VIEWPORT_MANAGER_H__
#define __OPENGL_3D_VIEWPORT_MANAGER_H__

#include <string>
#include <vector>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <map>

namespace Nova {
    
    class Camera;
    class CameraControlBase;
    class ApplicationFactory;
    class IOEvent;

    class CameraFactory {
    public:
        CameraFactory(){};
        virtual ~CameraFactory(){};        
        virtual std::unique_ptr<CameraControlBase> BuildCameraController(Camera& camera) = 0;
    };
    
    template<class ControllerType>
    class SimpleCameraFactory : public CameraFactory{
    public:
        SimpleCameraFactory() : CameraFactory() {};
        virtual ~SimpleCameraFactory() {};        
        virtual std::unique_ptr<CameraControlBase> BuildCameraController(Camera& camera) {
            return std::move( std::unique_ptr<CameraControlBase>( new ControllerType(camera) ));
        };
    };

    class ViewportManager {
    public:
        enum ViewportConfiguration { VM_SINGLE_VIEWPORT, VM_DUAL_VIEWPORT};//, VM_QUAD_VIEWPORT };

        ViewportManager( ApplicationFactory& app );
        virtual ~ViewportManager();

        // These methods manage the global viewport state
        unsigned int NumViewports();
        unsigned int CurrentViewport();
        void SetWindowGeometry( int width, int height );
        void SetViewport(unsigned int viewport);
        void ConfigureViewports(ViewportConfiguration wc);
        std::vector<std::string> AvailableControllers();
        void ResetAll(); // Reset all controllers for all viewports
        void HandleEvent( IOEvent& event );
        void Set_Controller( std::string controller, unsigned int viewport );
        void InitializeChrome();
        void DrawFrame();
        void Set_ClipBounds( float near, float far, unsigned int viewport );
        void Set_Ortho( bool ortho, unsigned int viewport );
        void Toggle_Ortho(unsigned int viewport );
        
        // These methods all act upon the CurrentViewport();
        void Update();
        glm::mat4 GetViewMatrix() const; 
        glm::mat4 GetModelMatrix() const;
        glm::mat4 GetProjectionMatrix() const;
        glm::vec4 GetViewport() const;
        void DrawAxis();

        

    private:
        enum AxisMode { AXIS_DISABLE=0, AXIS_CENTER=1, AXIS_CORNER=2, AXIS_MAX_VALUE=3 };
        
        struct Viewport {
            Viewport() = default;
            Viewport(Viewport&&) = default;
            Viewport(const Viewport&) = delete;
            Viewport& operator=(const Viewport&) = delete;
            int x;
            int y;
            int width;
            int height;
            float clipNear;
            float clipFar;
            std::unique_ptr<Camera> camera;
            std::unique_ptr<CameraControlBase> controller;            
        };

        void UpdateViewportGeometry( unsigned int viewport );
        
        unsigned int chromeVAO, chromeVBO;
        unsigned int axisVAO, axisVBO;
        ViewportConfiguration _currentConfiguration;
        glm::vec2 _geometry;
        std::map< std::string, std::unique_ptr<CameraFactory> > _controllerFactories; 
        std::vector< Viewport > _viewports;
        unsigned int _activeViewport;
        unsigned int _recentlyInteractedViewport;
        bool _focused;
        unsigned int _focusedViewport;
        unsigned int axis_mode;
        
        ApplicationFactory& _app;

    };

}

#endif
