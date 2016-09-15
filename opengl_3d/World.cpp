//#####################################################################
// Copyright (c) 2016, Mridul Aanjaneya.
//#####################################################################
#include "World.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "ApplicationFactory.h"

#include <nova/Rendering/OpenGL/Cameras/StaticCameraControl.h>
#include <nova/Rendering/OpenGL/Cameras/TrackballCameraControl.h>
#include <nova/Rendering/OpenGL/Cameras/OrbitCameraControl.h>


using namespace Nova;
//#####################################################################
// Constructor
//#####################################################################
World::
World(ApplicationFactory& app)
    :_app(app),window(nullptr),control(nullptr)
{
    render_camera.Set_Mode(FREE);
    render_camera.Set_Position(glm::vec3(0,0,-10));
    render_camera.Set_Look_At(glm::vec3(0,0,0));
    render_camera.Set_Clipping(.1,1000);
    render_camera.Set_FOV(45);
    control = new OrbitCameraControl( render_camera );
}
//#####################################################################
// Destructor
//#####################################################################
World::
~World()
{
    glfwTerminate();
    if( control ){
        delete control;
        control = nullptr;
    }
}
//#####################################################################
// Camera Matrix Operators
//#####################################################################
    glm::mat4 
    World::Get_ViewMatrix() const {
return render_camera.Get_ViewMatrix();
}

        glm::mat4
        World::Get_ModelMatrix() const {
return render_camera.Get_ModelMatrix();

}

            glm::mat4
            World::Get_ProjectionMatrix() const {
return render_camera.Get_ProjectionMatrix();

}

//#####################################################################
// Initialize
//#####################################################################
void World::
Initialize(int size_x,int size_y)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    window=glfwCreateWindow(size_x,size_y,"OpenGL Visualization",nullptr,nullptr);
    if(window==nullptr)
    {
        std::cout<<"Failed to create GLFW window!"<<std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);

    // disable vsync
    glfwSwapInterval(0);

    glewExperimental=GL_TRUE;
    if(glewInit()!=GLEW_OK)
    {
        std::cout<<"Failed to initialize GLEW!"<<std::endl;
        return;
    }

    glfwSetWindowCloseCallback(window,Close_Callback);
    glfwSetFramebufferSizeCallback(window,Reshape_Callback);
    glfwSetKeyCallback(window,Keyboard_Callback);
    glfwSetMouseButtonCallback(window,Mouse_Button_Callback);
    glfwSetCursorPosCallback(window,Mouse_Position_Callback);
    glfwSetScrollCallback(window,Scroll_Callback);

    glfwGetFramebufferSize(window,&size_x,&size_y);
    if(size_y>0)
    {
        window_size=glm::ivec2(size_x,size_y);
        window_aspect=float(size_x)/float(size_y);

        GLFWmonitor* primary=glfwGetPrimaryMonitor();
        glfwGetMonitorPhysicalSize(primary,&window_physical_size.x,&window_physical_size.y);
        glfwGetMonitorPos(primary,&window_position.x,&window_position.y);
        const GLFWvidmode* mode=glfwGetVideoMode(primary);
        dpi=mode->width/(window_physical_size.x/25.4);
    }

    glfwSetWindowUserPointer(window,this);
    glfwSetInputMode(window,GLFW_CURSOR_DISABLED,GL_FALSE);
    Initialize_Camera_Controls();
}
//#####################################################################
// Initialize_Camera_Controls
//#####################################################################
void World::
Initialize_Camera_Controls()
{
  int width, height;
  glfwGetWindowSize(window, &width, &height);

  // Fire a redraw event to initialize the control to the window size,
  // just in case it needs it. 
  OpenGL_IOEvent event;
  event.type = OpenGL_IOEvent::DRAW;
  event.draw_data.width = width;
  event.draw_data.height = height;
  event.currentTime = glfwGetTime();
  control->Redraw( event );

  // Call reset to clear any state
  control->Reset();
}
//#####################################################################
// Handle_Input
//#####################################################################
void World::
Handle_Input(int key)
{
}
//#####################################################################
// Main_Loop
//#####################################################################
void World::
Main_Loop()
{
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_CULL_FACE);

        glClearColor(18.0f/255.0f,26.0f/255.0f,32.0f/255.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render_camera.aspect=window_aspect;
        render_camera.window_width=window_size.x;
        render_camera.window_height=window_size.y;
        
        OpenGL_IOEvent event;
        event.type = OpenGL_IOEvent::TIME;
        event.currentTime = glfwGetTime();
        control->Update(event);
        render_camera.Update();

        glm::mat4 projection,view,model;
        render_camera.Get_Matrices(projection,view,model);

        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

        //GLuint projection_location=glGetUniformLocation(shader->program,"projection");
        //glUniformMatrix4fv(projection_location,1,GL_FALSE,glm::value_ptr(projection));
        //GLuint view_location=glGetUniformLocation(shader->program,"view");
        //glUniformMatrix4fv(view_location,1,GL_FALSE,glm::value_ptr(view));
        //GLuint model_location=glGetUniformLocation(shader->program,"model");
        //glUniformMatrix4fv(model_location,1,GL_FALSE,glm::value_ptr(model));

        _app.GetScene().Draw();

        glfwSwapBuffers(window);
    }
}
//#####################################################################
