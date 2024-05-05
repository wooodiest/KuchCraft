#version 450 core

layout (location = 0) out vec4 Color;
layout (location = 1) out vec4 Normal;

in vec4 v_Color;
in vec2 v_TexCoord;
in flat float v_TexIndex;

uniform sampler2D u_Textures[#max_texture_slots];

void main()
{
	Color  = v_Color * texture(u_Textures[int(v_TexIndex)], v_TexCoord);
	Normal = vec4(0.0);
}