

add_library( plugin_SOILTextureProvider
  SOILTextureProvider.cpp
)

find_package(SOIL REQUIRED)

if(WIN32)
  target_compile_definitions( plugin_SOILTextureProvider PUBLIC OPENGL_3D_WIN32=1)
endif()

if(UNIX)
  target_compile_definitions( plugin_SOILTextureProvider PUBLIC OPENGL_3D_LINUX=1)
endif()


target_link_libraries( plugin_SOILTextureProvider
  PRIVATE
  SOIL
)

target_compile_features( plugin_SOILTextureProvider
  PRIVATE
  cxx_auto_type
  )

