#version 450 core

layout (location = 0) out vec4 out_Color;
layout (location = 1) out vec4 out_Normal;

uniform samplerCube u_Texture;

in vec3 v_TexCoord;

void main()
{
	out_Color  = texture(u_Texture, v_TexCoord);
	out_Normal = vec4(0.0);
}