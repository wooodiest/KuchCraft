#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;
layout (location = 2) in float a_TexIndex;

layout (std140, binding = 0) uniform UniformCameraData
{
	mat4 u_ViewProjection;
	mat4 u_AbsoluteViewProjection;
	mat4 u_OrthoProjection;
};

out vec2 v_TexCoord;
out flat float v_TexIndex;

void main()
{
	v_TexCoord     = a_TexCoord;
	v_TexIndex     = a_TexIndex;

	gl_Position    = u_ViewProjection * vec4(a_Position, 1.0);
}