

add_library( plugin_FreeTypeRenderer
  FreeTypeRenderer.cpp
)



find_package( Freetype REQUIRED )


if(WIN32)
  target_compile_definitions( plugin_FreeTypeRenderer PUBLIC OPENGL_3D_WIN32=1)
endif()

if(UNIX)
  target_compile_definitions( plugin_FreeTypeRenderer PUBLIC OPENGL_3D_LINUX=1)
endif()

target_include_directories(plugin_FreeTypeRenderer PRIVATE
${FREETYPE_INCLUDE_DIRS}
  )

target_link_libraries( plugin_FreeTypeRenderer
  PRIVATE
  nova::rendering
  ${FREETYPE_LIBRARIES}
)

target_compile_features( plugin_FreeTypeRenderer
  PRIVATE
  cxx_auto_type
  )
