struct ChunkVertexData
{
    vec4 Position;
    vec3 Normal;
    vec2 UV;
};

const ChunkVertexData chunk_vertex_data[24] = ChunkVertexData[](

    /* bottom */
    ChunkVertexData(
        vec4( 1.0,  0.0,  1.0,  1.0 ),
        vec3( 0.0, -1.0,  0.0       ),
        vec2( 0.25, 0.5             )
    ),
    ChunkVertexData(
        vec4( 0.0,  0.0,  1.0,  1.0 ),
        vec3( 0.0, -1.0,  0.0       ),
        vec2( 0.0,  0.5             )
    ),
    ChunkVertexData(
        vec4( 0.0,  0.0,  0.0,  1.0 ),
        vec3( 0.0, -1.0,  0.0       ),
        vec2( 0.0,  0.0             )
    ),
    ChunkVertexData(
        vec4( 1.0,  0.0,  0.0,  1.0 ),
        vec3( 0.0, -1.0,  0.0       ),
        vec2( 0.25, 0.0             )
    ),

    /* top */
    ChunkVertexData(
        vec4( 0.0,  1.0,  1.0,  1.0 ),
        vec3( 0.0,  1.0,  0.0       ),
        vec2( 0.25, 0.0             )
    ),
    ChunkVertexData(
        vec4( 1.0,  1.0,  1.0,  1.0 ),
        vec3( 0.0,  1.0,  0.0       ),
        vec2( 0.5,  0.0             )
    ),
    ChunkVertexData(
        vec4( 1.0,  1.0,  0.0,  1.0 ),
        vec3( 0.0,  1.0,  0.0       ),
        vec2( 0.5,  0.5             )
    ),
    ChunkVertexData(
        vec4( 0.0,  1.0,  0.0,  1.0 ),
        vec3( 0.0,  1.0,  0.0       ),
        vec2( 0.25, 0.5             )
    ),

    /* front */
    ChunkVertexData(
        vec4( 0.0,  0.0,  1.0,  1.0 ),
        vec3( 0.0,  0.0,  1.0       ),
        vec2( 0.0,  0.5             )
    ),
    ChunkVertexData(
        vec4( 1.0,  0.0,  1.0,  1.0 ),
        vec3( 0.0,  0.0,  1.0       ),
        vec2( 0.25, 0.5             )
    ),
    ChunkVertexData(
        vec4( 1.0,  1.0,  1.0,  1.0 ),
        vec3( 0.0,  0.0,  1.0       ),
        vec2( 0.25, 1.0             )
    ),
    ChunkVertexData(
        vec4( 0.0,  1.0,  1.0,  1.0 ),
        vec3( 0.0,  0.0,  1.0       ),
        vec2( 0.0,  1.0             )
    ),

    /* right */
    ChunkVertexData(
        vec4( 1.0,  0.0,  1.0,  1.0 ),
        vec3( 1.0,  0.0,  0.0       ),
        vec2( 0.25, 0.5             )
    ),
    ChunkVertexData(
        vec4( 1.0,  0.0,  0.0,  1.0 ),
        vec3( 1.0,  0.0,  0.0       ),
        vec2( 0.5,  0.5             )
    ),
    ChunkVertexData(
        vec4( 1.0,  1.0,  0.0,  1.0 ),
        vec3( 1.0,  0.0,  0.0       ),
        vec2( 0.5,  1.0             )
    ),
    ChunkVertexData(
        vec4( 1.0,  1.0,  1.0,  1.0 ),
        vec3( 1.0,  0.0,  0.0       ),
        vec2( 0.25, 1.0             )
    ),

    /* behind */
    ChunkVertexData(
        vec4( 1.0,  0.0,  0.0,  1.0 ),
        vec3( 0.0 , 0.0, -1.0       ),
        vec2( 0.5,  0.5             )
    ),
    ChunkVertexData(
        vec4( 0.0,  0.0,  0.0,  1.0 ),
        vec3( 0.0 , 0.0, -1.0       ),
        vec2( 0.75, 0.5             )
    ),
    ChunkVertexData(
        vec4( 0.0,  1.0,  0.0,  1.0 ),
        vec3( 0.0 , 0.0, -1.0       ),
        vec2( 0.75, 1.0             )
    ),
    ChunkVertexData(
        vec4( 1.0,  1.0,  0.0,  1.0 ),
        vec3( 0.0 , 0.0, -1.0       ),
        vec2( 0.5,  1.0             )
    ),

    /* left */
    ChunkVertexData(
        vec4( 0.0,  0.0,  0.0,  1.0 ),
        vec3(-1.0,  0.0,  0.0       ),
        vec2( 0.75, 0.5             )
    ),
    ChunkVertexData(
        vec4( 0.0,  0.0,  1.0,  1.0 ),
        vec3(-1.0,  0.0,  0.0       ),
        vec2( 1.0,  0.5             )
    ),
    ChunkVertexData(
        vec4( 0.0,  1.0,  1.0,  1.0 ),
        vec3(-1.0,  0.0,  0.0       ),
        vec2( 1.0,  1.0             )
    ),
    ChunkVertexData(
        vec4( 0.0,  1.0,  0.0,  1.0 ),
        vec3(-1.0,  0.0,  0.0       ),
        vec2( 0.75, 1.0             )
    )
);

const vec2 rotated_uv[32] = vec2[](
    /* bottom */
    vec2(0.25, 0.5 ),
    vec2(0.25, 0.0 ),
    vec2(0.0,  0.0 ),
    vec2(0.0 , 0.5 ),

    vec2(0.0,  0.5 ),
    vec2(0.25, 0.5 ),
    vec2(0.25, 0.0 ),
    vec2(0.0,  0.0 ),

    vec2(0.0,  0.0 ),
    vec2(0.0,  0.5 ),
    vec2(0.25, 0.5 ),
    vec2(0.25, 0.0 ),

    vec2(0.25, 0.0 ),
    vec2(0.0,  0.0 ),
    vec2(0.0,  0.5 ),
    vec2(0.25, 0.5 ),

    /* top */
    vec2(0.25, 0.0),
    vec2(0.5,  0.0),
    vec2(0.5,  0.5),
    vec2(0.25, 0.5),

    vec2(0.5,  0.0),
    vec2(0.5,  0.5),
    vec2(0.25, 0.5),
    vec2(0.25, 0.0),

    vec2(0.5,  0.5),
    vec2(0.25, 0.5),
    vec2(0.25, 0.0),
    vec2(0.5,  0.0),

    vec2(0.25, 0.5),
    vec2(0.25, 0.0),
    vec2(0.5,  0.0),
    vec2(0.5,  0.5)
);