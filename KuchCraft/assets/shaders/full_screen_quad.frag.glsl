#version 450 core

layout (location = 0) out vec4 out_Color;

uniform sampler2D u_Texture;

in vec2 v_TexCoord;

void main()
{
	vec4 color = texture(u_Texture, v_TexCoord);
	
	if (color.a == 0.0)
		discard;

	out_Color = color;
}