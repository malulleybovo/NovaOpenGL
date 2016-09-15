add_executable(opengl_3d_new 
  main.cpp
  Config.cpp
  ApplicationFactory.cpp
  ShaderManager.cpp
  Scene.cpp
  PluginManager.cpp
  Plugin.cpp
  RenderableManager.cpp
  World.cpp
  Shader.cpp
  )

pkg_check_modules (glfw3 REQUIRED glfw3)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Boost 1.54 REQUIRED COMPONENTS filesystem program_options)

target_link_libraries( opengl_3d_new
  ${Boost_LIBRARIES}
  nova::rendering
  ${CMAKE_DL_LIBS}
  SOIL ${OPENGL_LIBRARIES} glfw GLEW
  )

target_compile_features( opengl_3d_new
  PRIVATE
  cxx_auto_type
)

#
#  Now all the plugins...
#
find_package(assimp REQUIRED)

add_library( plugin_NovaMesh
  plugins/NovaMesh/NovaMesh.cpp
  plugins/NovaMesh/NovaMesh_Model.cpp
  plugins/NovaMesh/NovaMesh_Mesh.cpp
)

target_link_libraries( plugin_NovaMesh
  PRIVATE
  nova::rendering
  ${Boost_LIBRARIES}
  ${assimp_LIBRARIES}
)

target_compile_features( plugin_NovaMesh
  PRIVATE
  cxx_auto_type
  )
