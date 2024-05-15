#version 450 core

layout (location = 0) out vec4 out_Color;

in vec4 v_Color;
in vec2 v_TexCoord;
in flat float v_TexIndex;

uniform sampler2D u_Textures[#max_texture_slots];

void main()
{
	vec4 color = v_Color * texture(u_Textures[int(v_TexIndex)], v_TexCoord);

	if (color.a == 0.0)
		discard;

	out_Color = color;
}