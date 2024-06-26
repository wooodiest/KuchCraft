#version 450 core

layout (location = 0) out vec4 out_Color;
layout (location = 1) out vec4 out_Normal;

uniform sampler2DArray u_Texture;

struct TextData
{
	mat4 Transform;
	vec4 Color;
	vec4 Letter;
};

layout (std140, binding = 2) uniform UniformTextData
{
	TextData u_Text[#max_text_uniform_array_limit];
};

in vec2 v_TexCoord;
in flat int v_Index;

void main()
{
    out_Color  = u_Text[v_Index].Color * vec4(1.0, 1.0, 1.0, texture(u_Texture, vec3(v_TexCoord.xy, u_Text[v_Index].Letter.x)).r);
	out_Normal = vec4(0.0);
}