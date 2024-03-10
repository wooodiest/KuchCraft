#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;

##world_data_uniform_buffer

out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}