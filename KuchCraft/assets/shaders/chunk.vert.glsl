#version 450 core

layout (location = 0) in uint a_PackedData;

##world_data_uniform_buffer

uniform vec3 u_ChunkPosition;

out vec2 v_TexCoord;
out flat uint v_TexIndex;

const vec4 vertexPositions[24] = vec4[](
	/* bottom */
    vec4(1.0, 0.0, 1.0, 1.0),
    vec4(0.0, 0.0, 1.0, 1.0),
	vec4(0.0, 0.0, 0.0, 1.0),
	vec4(1.0, 0.0, 0.0, 1.0),

	/* top */
	vec4(0.0, 1.0, 1.0, 1.0),
    vec4(1.0, 1.0, 1.0, 1.0),
	vec4(1.0, 1.0, 0.0, 1.0),
	vec4(0.0, 1.0, 0.0, 1.0),

	/* front */
	vec4(0.0, 0.0, 1.0, 1.0),
    vec4(1.0, 0.0, 1.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),
	vec4(0.0, 1.0, 1.0, 1.0),

	/* right */
	vec4(1.0, 0.0, 1.0, 1.0),
    vec4(1.0, 0.0, 0.0, 1.0),
	vec4(1.0, 1.0, 0.0, 1.0),
	vec4(1.0, 1.0, 1.0, 1.0),

	/* behind */
	vec4(1.0, 0.0, 0.0, 1.0),
    vec4(0.0, 0.0, 0.0, 1.0),
	vec4(0.0, 1.0, 0.0, 1.0),
	vec4(1.0, 1.0, 0.0, 1.0),

	/* left */
	vec4(0.0, 0.0, 0.0, 1.0),
    vec4(0.0, 0.0, 1.0, 1.0),
	vec4(0.0, 1.0, 1.0, 1.0),
	vec4(0.0, 1.0, 0.0, 1.0)
);

const vec2 texCoords[24] = vec2[](
	/* bottom */
    vec2(0.25, 0.0),
    vec2(0.0,  0.0),
	vec2(0.0,  0.5),
	vec2(0.25, 0.5),

	/* top */
	vec2(0.25, 0.0),
    vec2(0.5,  0.0),
	vec2(0.5,  0.5),
	vec2(0.25, 0.5),

	/* front */
	vec2(0.0,  0.5),
    vec2(0.25, 0.5),
	vec2(0.25, 1.0),
	vec2(0.0,  1.0),

	/* right */
	vec2(0.25, 0.5),
    vec2(0.5,  0.5),
	vec2(0.5,  1.0),
	vec2(0.25, 1.0),

	/* behind */
	vec2(0.5,  0.5),
    vec2(0.75, 0.5),
	vec2(0.75, 1.0),
	vec2(0.5,  1.0),

	/* left */
	vec2(0.75, 0.5),
    vec2(1.0,  0.5),
	vec2(1.0,  1.0),
	vec2(0.75, 1.0)
);

mat4 TranslationMatrix(vec3 translation)
{
    return mat4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(translation  , 1.0)
    );
}

void main()
{
	//      a_PackedData        //
	//   5B - position.x | 0B	//
	//   7B - position.y | 5B	//
	//   5B - position.z | 12B  //
	//   5B - index      | 17B  //
	//   7B - texture    | 22B  //

	uint index      = (a_PackedData >> 17 ) & 0x1F; 
	mat4 transform  = TranslationMatrix(vec3(
							u_ChunkPosition.x + float((a_PackedData       ) & 0x1F),
							u_ChunkPosition.y + float((a_PackedData >> 5  ) & 0x7F),
							u_ChunkPosition.z + float((a_PackedData >> 12 ) & 0x1F))
						);

	v_TexCoord  = texCoords[index];
	v_TexIndex  = (a_PackedData >> 22 ) & 0x7F;
	gl_Position = u_ViewProjection * transform * vertexPositions[index];
}