#version 450 core

layout (location = 0) out vec4 color;

uniform samplerCube u_Texture;
layout(std140, binding = 0) uniform UniformData
{
	mat4 u_ViewProjection;
	mat4 u_AbsoluteViewProjection;
	mat4 u_OrthoProjection;
	vec4 u_TintColor;
};

in vec3 v_TexCoord;

void main()
{
	color = texture(u_Texture, v_TexCoord) * u_TintColor;
}