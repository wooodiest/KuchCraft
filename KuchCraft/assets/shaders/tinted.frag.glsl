#version 450 core

layout (location = 0) out vec4 out_Color;

uniform sampler2D u_Texture;
uniform vec4 u_Color;

in vec2 v_TexCoord;

void main()
{
	out_Color = texture(u_Texture, v_TexCoord) * u_Color;
}