#version 450 core

layout (location = 0) out vec4 Color;
layout (location = 1) out vec4 Normal;

uniform samplerCube u_Texture;

in vec3 v_TexCoord;

void main()
{
	Color  = texture(u_Texture, v_TexCoord);
	Normal = vec4(0.0);
}