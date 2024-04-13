#version 450 core

#include "assets/shaders/chunk_info.glsl"
#include "assets/shaders/utils/matrix_transformations.glsl"

layout (location = 0) in uint a_PackedData;

layout(std140, binding = 0) uniform UniformCameraData
{
	mat4 u_ViewProjection;
	mat4 u_AbsoluteViewProjection;
	mat4 u_OrthoProjection;
};

uniform vec3 u_ChunkPosition;

out vec2 v_TexCoord;
out flat uint v_TexIndex;

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

	v_TexCoord  = chunk_vertex_data[index].UV;
	v_TexIndex  = (a_PackedData >> 22 ) & 0x7F;
	gl_Position = u_ViewProjection * transform * chunk_vertex_data[index].Position;
}