#version 450 core

layout (location = 0) out vec4 color;

uniform sampler2D u_Textures[##max_texture_slots];
##world_data_uniform_buffer

in vec2 v_TexCoord;
in flat float v_TexIndex;

void main()
{
	color = texture(u_Textures[int(v_TexIndex)], v_TexCoord) * u_TintColor;
}