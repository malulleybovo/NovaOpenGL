add_library( opengl_3d_kernel
  Config.cpp
  ApplicationFactory.cpp
  ShaderManager.cpp
  Scene.cpp
  PluginManager.cpp
  Plugin.cpp
  RenderableManager.cpp
  World.cpp
  Shader.cpp
  IOService.cpp
  TextRenderingService.cpp
  )

pkg_check_modules (glfw3 REQUIRED glfw3)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Boost 1.54 REQUIRED COMPONENTS filesystem program_options)

if(WIN32)
  target_compile_definitions(opengl_3d_kernel PUBLIC OPENGL_3D_WIN32=1)
endif()

if(UNIX)
  target_compile_definitions(opengl_3d_kernel PUBLIC OPENGL_3D_LINUX=1)
endif()


target_link_libraries( opengl_3d_kernel
  ${Boost_LIBRARIES}
  nova::rendering
  ${CMAKE_DL_LIBS}
  SOIL ${OPENGL_LIBRARIES} glfw GLEW
  )

target_compile_features( opengl_3d_kernel
  PUBLIC
  cxx_auto_type
)

#
#  Now the main program
#

add_executable(opengl_3d_new 
  main.cpp
  )

target_link_libraries( opengl_3d_new
  opengl_3d_kernel )

#
#  Now all the plugins...
#

add_subdirectory( plugins/Assimp )
add_subdirectory( plugins/FreeTypeTextRenderer )
