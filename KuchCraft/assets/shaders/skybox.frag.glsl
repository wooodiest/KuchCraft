#version 450 core

layout (location = 0) out vec4 color;

uniform samplerCube u_Texture;
##world_data_uniform_buffer

in vec3 v_TexCoord;

void main()
{
	color = texture(u_Texture, v_TexCoord) * u_TintColor;
}