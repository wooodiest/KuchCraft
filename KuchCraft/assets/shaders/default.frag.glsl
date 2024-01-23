#version 450

layout (location = 0) out vec4 color;

in vec2 v_TexCoord;
in flat float v_TexIndex;

uniform sampler2D u_Textures[48];

void main()
{
	//color = texture(u_Textures[0], v_TexCoord);
	color = vec4(1.0, 0.0, 0.0, 1.0);
}