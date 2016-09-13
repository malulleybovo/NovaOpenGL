add_executable(opengl_3d_new 
  main.cpp
  Config.cpp
  ApplicationFactory.cpp
  ShaderManager.cpp
  )

find_package(Boost 1.54 REQUIRED COMPONENTS filesystem program_options)
  
target_link_libraries( opengl_3d_new
  ${Boost_LIBRARIES}
  nova::rendering
  )

target_compile_features( opengl_3d_new
  PRIVATE
  cxx_auto_type
)
