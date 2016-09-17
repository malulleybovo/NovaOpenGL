

add_library( plugin_Assimp
  AssimpRenderable.cpp
  AssimpRenderable_Model.cpp
  AssimpRenderable_Mesh.cpp
)


find_package(assimp REQUIRED)
find_package(Boost 1.54 REQUIRED COMPONENTS filesystem program_options)


if(WIN32)
  target_compile_definitions( plugin_Assimp PUBLIC OPENGL_3D_WIN32=1)
endif()

if(UNIX)
  target_compile_definitions( plugin_Assimp PUBLIC OPENGL_3D_LINUX=1)
endif()


target_link_libraries( plugin_Assimp
  PRIVATE
  nova::rendering
  ${Boost_LIBRARIES}
  ${assimp_LIBRARIES}
)

target_compile_features( plugin_Assimp
  PRIVATE
  cxx_auto_type
  )
