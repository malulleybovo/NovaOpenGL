//#####################################################################
// Copyright (c) 2016, Mridul Aanjaneya.
//#####################################################################
// Class World
//##################################################################### 
#ifndef __World__
#define __World__

#include <nova/Rendering/OpenGL/Cameras/Camera.h>
#include <nova/Rendering/OpenGL/Cameras/CameraControlInterface.h>
#include <nova/Rendering/OpenGL/Utilities/IOEvents.h>
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
        OpenGL_CameraControlBase* control;
        
    public:
        World(ApplicationFactory& app);
        ~World();
        void Initialize_Camera_Controls();
        
        glm::mat4 Get_ViewMatrix() const ;
        glm::mat4 Get_ModelMatrix() const ;
        glm::mat4 Get_ProjectionMatrix() const ; 


        inline static void Close_Callback(GLFWwindow* window)
        {glfwSetWindowShouldClose(window,GL_TRUE);}
        
        inline static void Scroll_Callback( GLFWwindow* window,double xoffset,double yoffset)
        {
            World *world=static_cast<World*>(glfwGetWindowUserPointer(window));
            
            OpenGL_IOEvent event;
            event.type = OpenGL_IOEvent::SCROLL;
            event.scroll_data.x = xoffset;
            event.scroll_data.y = yoffset;
            event.currentTime = glfwGetTime();
            world->control->Scroll( event );
            
        }
        
        
        inline static void Reshape_Callback(GLFWwindow* window,int w,int h)
        {
            World *world=static_cast<World*>(glfwGetWindowUserPointer(window));
            if(h>0)
                {
                    world->window_size=glm::ivec2(w,h);
                    world->window_aspect=float(w)/float(h);
                }
            
            GLFWmonitor *primary=glfwGetPrimaryMonitor();
            glfwGetMonitorPhysicalSize(primary,&world->window_physical_size.x,&world->window_physical_size.y);
            glfwGetMonitorPos(primary,&world->window_position.x,&world->window_position.y);
            const GLFWvidmode *mode=glfwGetVideoMode(primary);
            world->dpi=mode->width/(world->window_physical_size.x/25.4);
            
            OpenGL_IOEvent event;
            event.type = OpenGL_IOEvent::DRAW;
            event.draw_data.width = w;
            event.draw_data.height = h;
            event.currentTime = glfwGetTime();
            world->control->Redraw( event );
        }
        
        inline static void Keyboard_Callback(GLFWwindow* window,int key,int scancode,int action,int mode)
        {
            World *world=static_cast<World*>(glfwGetWindowUserPointer(window));
            if(key==GLFW_KEY_ESCAPE && action==GLFW_PRESS)
                glfwSetWindowShouldClose(window,GL_TRUE);           
            
            OpenGL_IOEvent event;
            event.type = OpenGL_IOEvent::KEYBOARD;
            event.key_data.key = OpenGL_IOEvent::KEY_CODE(key);
            event.key_data.scancode = scancode;
            event.key_data.mods =
                ( (mode & GLFW_MOD_SUPER != 0 ) ?  OpenGL_IOEvent::mSHIFT : 0 )  | 
                ( (mode & GLFW_MOD_ALT != 0 ) ?  OpenGL_IOEvent::mALT : 0 )  | 
                ( (mode & GLFW_MOD_CONTROL != 0 ) ?  OpenGL_IOEvent::mCONTROL : 0 )  | 
                ( (mode & GLFW_MOD_SUPER != 0 ) ?  OpenGL_IOEvent::mSUPER : 0  );
            event.currentTime = glfwGetTime();
            switch( action ){
            case GLFW_PRESS:
                event.key_data.action = OpenGL_IOEvent::K_DOWN;
                world->control->KeyDown( event );
                break;
            case GLFW_RELEASE:
                event.key_data.action = OpenGL_IOEvent::K_UP;
                world->control->KeyUp( event );
                break;
            case GLFW_REPEAT:
                event.key_data.action = OpenGL_IOEvent::K_HOLD;
                world->control->KeyHold( event );
                break;
            }
        }
        
        inline static void Mouse_Button_Callback(GLFWwindow* window,int button,int state,int mods)
        {
            World *world=static_cast<World*>(glfwGetWindowUserPointer(window));
            double x,y;
            glfwGetCursorPos(window,&x,&y);
            
            OpenGL_IOEvent event;
            event.type = OpenGL_IOEvent::MOUSEBUTTON;
            event.currentTime = glfwGetTime();
            switch( button ){
            case GLFW_MOUSE_BUTTON_LEFT:
                event.mousebutton_data.button = OpenGL_IOEvent::M_LEFT;
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                event.mousebutton_data.button = OpenGL_IOEvent::M_RIGHT;
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                event.mousebutton_data.button = OpenGL_IOEvent::M_MIDDLE;
                break;
            default:
                event.mousebutton_data.button = OpenGL_IOEvent::M_OTHER;
                break;
            }
            event.mousebutton_data.button_raw = button;
            event.mousebutton_data.x = x;
            event.mousebutton_data.y = y;
            event.mousebutton_data.mods =
                ( (mods & GLFW_MOD_SUPER != 0 ) ?  OpenGL_IOEvent::mSHIFT : 0 )  | 
                ( (mods & GLFW_MOD_ALT != 0 ) ?  OpenGL_IOEvent::mALT : 0 )  | 
                ( (mods & GLFW_MOD_CONTROL != 0 ) ?  OpenGL_IOEvent::mCONTROL : 0 )  | 
                ( (mods & GLFW_MOD_SUPER != 0 ) ?  OpenGL_IOEvent::mSUPER : 0  );
            switch( state ){
            case GLFW_PRESS:
                event.mousebutton_data.action = OpenGL_IOEvent::M_DOWN;
                world->control->MouseDown( event );
                break;
            case GLFW_RELEASE:
                event.mousebutton_data.action = OpenGL_IOEvent::M_UP;
                world->control->MouseUp( event );
                break;
            }
        }
        
        inline static void Mouse_Position_Callback(GLFWwindow* window,double x,double y)
        {
            World *world=static_cast<World*>(glfwGetWindowUserPointer(window));
            OpenGL_IOEvent event;
            event.type = OpenGL_IOEvent::MOUSEMOVE;
            event.currentTime = glfwGetTime();
            event.mousemotion_data.x = x;
            event.mousemotion_data.y = y;
            world->control->MouseMove( event );
        }
        
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
