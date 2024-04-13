#version 450 core

layout (location = 0) out vec4 color;

uniform sampler2D u_Textures[#max_texture_slots];

in vec2 v_TexCoord;
in flat uint v_TexIndex;

void main()
{
	color = texture(u_Textures[v_TexIndex], v_TexCoord);
}