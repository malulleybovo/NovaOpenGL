//#####################################################################
// Copyright (c) 2016, Mridul Aanjaneya.
//#####################################################################
// Class World
//##################################################################### 
#ifndef __World__
#define __World__

#include "Camera.h"
#include "CameraControlInterface.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <string>

namespace Nova{
    class ApplicationFactory;
    class Scene;
    class World
    {
        GLFWwindow *window;
        
        ApplicationFactory& _app;        
        glm::ivec2 window_size,window_position,window_physical_size;
        double dpi;
        float window_aspect;
        Camera render_camera,ortho_camera;
        CameraControlBase* control;
        
    public:
        World(ApplicationFactory& app);
        ~World();
        void Initialize_Camera_Controls();
        
        glm::mat4 Get_ViewMatrix() const ;
        glm::mat4 Get_ModelMatrix() const ;
        glm::mat4 Get_ProjectionMatrix() const ; 
        glm::vec4 Get_Viewport() const;

        static void Close_Callback(GLFWwindow* window);
        static void Scroll_Callback( GLFWwindow* window,double xoffset,double yoffset);              
        static void Reshape_Callback(GLFWwindow* window,int w,int h);                
        static void Keyboard_Callback(GLFWwindow* window,int key,int scancode,int action,int mode);              
        static void Mouse_Button_Callback(GLFWwindow* window,int button,int state,int mods);                
        static void Mouse_Position_Callback(GLFWwindow* window,double x,double y);
        
        
        //inline void Set_Shader(Shader& shader_input)
        //{shader=&shader_input;}
        
//##################################################################### 
        void Initialize(int size_x,int size_y);
        void Handle_Input(int key);
        void Main_Loop();
//##################################################################### 
    };
}
#endif
