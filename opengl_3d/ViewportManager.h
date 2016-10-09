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
        virtual ~CameraFactory();        
        virtual std::unique_ptr<CameraControlBase> BuildCameraController(Camera& camera) = 0;
    };

    class ViewportManager {
    public:
        enum ViewportConfiguration { VM_SINGLE_VIEWPORT };//., VM_DUAL_VIEWPORT, VM_QUAD_VIEWPORT };

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

        // These methods all act upon the CurrentViewport();
        void Update();
        void Set_ClipBounds( float near, float far );
        void Set_Controller( std::string controller );
        void Set_Ortho( bool ortho );
        glm::mat4 GetViewMatrix() const; 
        glm::mat4 GetModelMatrix() const;
        glm::mat4 GetProjectionMatrix() const;
        glm::vec4 GetViewport() const;

        

    private:
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

        ViewportConfiguration _currentConfiguration;
        glm::vec2 _geometry;
        std::map< std::string, std::unique_ptr<CameraFactory> > _controllerFactories; 
        std::vector< Viewport > _viewports;
        unsigned int _activeViewport;
        unsigned int _recentlyInteractedViewport;
        bool _focused;
        unsigned int _focusedViewport;

        ApplicationFactory& _app;

    };

}

#endif
