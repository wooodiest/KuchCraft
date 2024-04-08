#version 450 core

layout (location = 0) out vec4 color;

uniform sampler2DArray u_Texture;
uniform vec4 u_Color;

struct TextData
{
	mat4 Transform;
	vec4 Letter;
};

layout(std140, binding = 2) uniform UniformTextData
{
	TextData u_Text[##max_text_uniform_array_limit];
};

in vec2 v_TexCoord;
in flat int v_Index;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_Texture, vec3(v_TexCoord.xy, u_Text[v_Index].Letter.x)).r);
    color = u_Color * sampled;
}