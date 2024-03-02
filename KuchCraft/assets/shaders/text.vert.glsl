#version 450 core

layout (location = 0) in vec2 a_Position;

uniform mat4 u_Transforms[200];
layout(std140, binding = 0) uniform UniformData
{
	mat4 u_ViewProjection;
	mat4 u_AbsoluteViewProjection;
	mat4 u_OrthoProjection;
	vec4 u_TintColor;
};

out vec2 v_TexCoord;
out flat int v_Index;

void main()
{
	gl_Position = u_OrthoProjection * u_Transforms[gl_InstanceID] * vec4(a_Position.xy, 0.0, 1.0);
    v_Index = gl_InstanceID;
    v_TexCoord = a_Position.xy;
    v_TexCoord.y = 1.0 - v_TexCoord.y;
}