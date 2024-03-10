#version 450 core

layout (location = 0) in vec2 a_Position;

##world_data_uniform_buffer
##text_data_uniform_buffer

out vec2 v_TexCoord;
out flat int v_Index;

void main()
{
	gl_Position = u_OrthoProjection * u_Text[gl_InstanceID].Transform * vec4(a_Position.xy, 0.0, 1.0);

    v_Index = gl_InstanceID;
	v_TexCoord = vec2(a_Position.x, 1.0 - a_Position.y);
}