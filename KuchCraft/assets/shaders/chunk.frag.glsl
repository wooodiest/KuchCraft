#version 450 core

layout (location = 0) out vec4 Color;
layout (location = 1) out vec4 Normal;

uniform sampler2D u_Textures[#max_texture_slots];

in vec2 v_TexCoord;
in flat uint v_TexIndex;
in vec3 v_Normal;

void main()
{
	Color  = texture(u_Textures[v_TexIndex], v_TexCoord);
	Normal = vec4(v_Normal, 0.0);
}