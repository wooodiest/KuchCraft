#version 450 core

layout (location = 0) in vec2 a_Position;

layout(std140, binding = 0) uniform UniformData
{
	mat4 u_ViewProjection;
	mat4 u_AbsoluteViewProjection;
	mat4 u_OrthoProjection;
	vec4 u_TintColor;
};

out vec2 v_TexCoord;
out flat int v_Index;

struct TextData
{
	mat4 Transform;
	vec4 Letter;
};

layout(std140, binding = 1) uniform UniformTextData
{
	TextData u_Text[400];
	vec4 u_Color;
};

void main()
{
	gl_Position = u_OrthoProjection * u_Text[gl_InstanceID].Transform * vec4(a_Position.xy, 0.0, 1.0);

    v_Index = gl_InstanceID;
	v_TexCoord = vec2(a_Position.x, 1.0 - a_Position.y);
}