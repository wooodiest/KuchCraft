#version 450 core

layout (location = 0) out vec4 out_Color;
layout (location = 1) out vec4 out_Normal;

uniform sampler2D u_Textures[#max_texture_slots];

in vec2 v_TexCoord;
in flat uint v_TexIndex;
in vec3 v_Normal;

void main()
{
	out_Color  = texture(u_Textures[v_TexIndex], v_TexCoord);
	out_Normal = vec4(v_Normal, 0.0);
}