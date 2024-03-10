#version 450 core

layout (location = 0) in vec3 a_Position;

##world_data_uniform_buffer

out vec3 v_TexCoord;

void main()
{
	v_TexCoord = a_Position;
	gl_Position = (u_AbsoluteViewProjection * vec4(a_Position, 1.0)).xyww;
}