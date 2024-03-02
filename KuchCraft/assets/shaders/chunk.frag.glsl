#version 450 core

layout (location = 0) out vec4 color;

uniform sampler2D u_Textures[32];
layout(std140, binding = 0) uniform UniformData
{
	mat4 u_ViewProjection;
	mat4 u_AbsoluteViewProjection;
	mat4 u_OrthoProjection;
	vec4 u_TintColor;
};

in vec2 v_TexCoord;
in flat float v_TexIndex;

void main()
{
	color = texture(u_Textures[int(v_TexIndex)], v_TexCoord) * u_TintColor;
}