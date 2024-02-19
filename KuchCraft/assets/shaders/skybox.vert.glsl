#version 450 core

layout (location = 0) in vec3 a_Position;

layout(std140, binding = 0) uniform UniformData
{
	mat4 u_ViewProjection;
	mat4 u_AbsoluteViewProjection;
	vec4 u_TintColor;
};

out vec3 v_TexCoord;

void main()
{
	v_TexCoord = a_Position;
	gl_Position = (u_AbsoluteViewProjection * vec4(a_Position, 1.0)).xyww;
}