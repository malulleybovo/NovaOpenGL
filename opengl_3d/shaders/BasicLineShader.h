namespace NovaBuiltinShaders{

    struct BasicLineShader {
        static constexpr char const * name = "BasicLineShader";
        static constexpr char const * fragment_shader = R"lang::GLSL(
#version 330 core
out vec4 color;

void main()
{    
    color=vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
)lang::GLSL";

        static constexpr char const * vertex_shader = R"lang::GLSL(
#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position=projection*view*model*vec4(position,1.0f);
}
)lang::GLSL";
        static constexpr char const * geometry_shader = nullptr;
            };
};
