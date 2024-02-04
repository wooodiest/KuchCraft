#version 450 core

layout (location = 0) in vec3 a_Position;

uniform mat4 u_ViewProjection0;

out vec3 v_TexCoord;

void main()
{
	v_TexCoord = a_Position;
	gl_Position = (u_ViewProjection0 * vec4(a_Position, 1.0)).xyww;
}