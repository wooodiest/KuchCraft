#version 450 core

layout (location = 0) out vec4 Color;

uniform sampler2D u_Texture;
uniform vec4 u_Color;

in vec2 v_TexCoord;

void main()
{
	Color = texture(u_Texture, v_TexCoord) * u_Color;
}