#version 450 core

layout (location = 0) out vec4 color;

uniform sampler2DArray u_Texture;
uniform int u_LetterMap[400];
uniform vec4 u_TextColor;

in vec2 v_TexCoord;
in flat int v_Index;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_Texture, vec3(v_TexCoord.xy, u_LetterMap[v_Index])).r);
    color = u_TextColor * sampled;
}