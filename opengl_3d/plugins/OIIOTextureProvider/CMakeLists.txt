

add_library( plugin_OIIOTextureProvider
  OIIOTextureProvider.cpp
)

find_package(OIIO REQUIRED)

if(WIN32)
  target_compile_definitions( plugin_OIIOTextureProvider PUBLIC OPENGL_3D_WIN32=1)
endif()

if(UNIX)
  target_compile_definitions( plugin_OIIOTextureProvider PUBLIC OPENGL_3D_LINUX=1)
endif()


target_link_libraries( plugin_OIIOTextureProvider
  PRIVATE
  ${OPENIMAGEIO_LIBRARY}
)

target_compile_features( plugin_OIIOTextureProvider
  PRIVATE
  cxx_auto_type
  )

