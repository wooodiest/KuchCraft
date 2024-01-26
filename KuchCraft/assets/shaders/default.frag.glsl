#version 450

layout (location = 0) out vec4 color;

in vec2 v_TexCoord;
in flat float v_TexIndex;

uniform sampler2D u_Textures[32];

void main()
{
	color = texture(u_Textures[int(v_TexIndex)], v_TexCoord);
}