#version 450 core

layout (location = 0) out vec4 Color;

in vec2 v_TexCoord;

const float border_radius = #outlined_block_border_radius;

void main()
{
	if ((v_TexCoord.x > border_radius && v_TexCoord.x < 1.0 - border_radius) &&
	     v_TexCoord.y > border_radius && v_TexCoord.y < 1.0 - border_radius)
	{
		discard;
	}
		
	Color = vec4(#outlined_block_border_color);
}