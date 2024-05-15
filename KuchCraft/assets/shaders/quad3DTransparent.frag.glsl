#version 450 core

layout (location = 0) out vec4 out_Color;
layout (location = 1) out vec4 out_Normal;

in vec2 v_TexCoord;
in flat float v_TexIndex;

uniform sampler2D u_Textures[#max_texture_slots];

void main()
{
	out_Color  = texture(u_Textures[int(v_TexIndex)], v_TexCoord);
	out_Normal = vec4(0.0);
}