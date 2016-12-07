namespace NovaBuiltinShaders{

    struct BasicMeshShader {
        static constexpr char const * name = "BasicMeshShader";
        static constexpr char const * fragment_shader = R"lang::GLSL(
#version 330 core
in vec2 TexCoord;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform int selected;

void main()
{    
    if( selected != 0 ){
        color.rgb=texture(texture_diffuse1,TexCoord).rgb * vec3(1.2,0.7,0.7);
        color.a = 1.0;
    }
    else
        color=vec4(texture(texture_diffuse1,TexCoord));
}
)lang::GLSL";

        static constexpr char const * vertex_shader = R"lang::GLSL(
#version 330 core
layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 tex_coord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat3 textureFlip;
    textureFlip[0] = vec3( 1,  0, 0);
    textureFlip[1] = vec3( 0, -1, 0);
    textureFlip[2] = vec3( 0,  0, 0);

    gl_Position=projection*view*model*vec4(position,1.0f);
    TexCoord=(textureFlip*vec3(tex_coord,0)).rg;
}
)lang::GLSL";
        static constexpr char const * geometry_shader = nullptr;
            };
};
