struct ChunkVertexData
{
    vec4 Position;
    vec4 Normal;
    vec2 UV;
};

const ChunkVertexData chunk_vertex_data[24] = ChunkVertexData[](

    /* bottom */
    ChunkVertexData(
        vec4( 1.0,  0.0,  1.0,  1.0 ),
        vec4( 0.0, -1.0,  0.0,  0.0 ),
        vec2( 0.25, 0.5             )
    ),
    ChunkVertexData(
        vec4( 0.0,  0.0,  1.0,  1.0 ),
        vec4( 0.0, -1.0,  0.0,  0.0 ),
        vec2( 0.0,  0.5             )
    ),
    ChunkVertexData(
        vec4( 0.0,  0.0,  0.0,  1.0 ),
        vec4( 0.0, -1.0,  0.0,  0.0 ),
        vec2( 0.0,  0.0             )
    ),
    ChunkVertexData(
        vec4( 1.0,  0.0,  0.0,  1.0 ),
        vec4( 0.0, -1.0,  0.0,  0.0 ),
        vec2( 0.25, 0.0             )
    ),

    /* top */
    ChunkVertexData(
        vec4( 0.0,  1.0,  1.0,  1.0 ),
        vec4( 0.0,  1.0,  0.0,  0.0 ),
        vec2( 0.25, 0.0             )
    ),
    ChunkVertexData(
        vec4( 1.0,  1.0,  1.0,  1.0 ),
        vec4( 0.0,  1.0,  0.0,  0.0 ),
        vec2( 0.5,  0.0             )
    ),
    ChunkVertexData(
        vec4( 1.0,  1.0,  0.0,  1.0 ),
        vec4( 0.0,  1.0,  0.0,  0.0 ),
        vec2( 0.5,  0.5             )
    ),
    ChunkVertexData(
        vec4( 0.0,  1.0,  0.0,  1.0 ),
        vec4( 0.0,  1.0,  0.0,  0.0 ),
        vec2( 0.25, 0.5             )
    ),

    /* front */
    ChunkVertexData(
        vec4( 0.0,  0.0,  1.0,  1.0 ),
        vec4( 0.0,  0.0,  1.0,  0.0 ),
        vec2( 0.0,  0.5             )
    ),
    ChunkVertexData(
        vec4( 1.0,  0.0,  1.0,  1.0 ),
        vec4( 0.0,  0.0,  1.0,  0.0 ),
        vec2( 0.25, 0.5             )
    ),
    ChunkVertexData(
        vec4( 1.0,  1.0,  1.0,  1.0 ),
        vec4( 0.0,  0.0,  1.0,  0.0 ),
        vec2( 0.25, 1.0             )
    ),
    ChunkVertexData(
        vec4( 0.0,  1.0,  1.0,  1.0 ),
        vec4( 0.0,  0.0,  1.0,  0.0 ),
        vec2( 0.0,  1.0             )
    ),

    /* right */
    ChunkVertexData(
        vec4( 1.0,  0.0,  1.0,  1.0 ),
        vec4( 1.0,  0.0,  0.0,  0.0 ),
        vec2( 0.25, 0.5             )
    ),
    ChunkVertexData(
        vec4( 1.0,  0.0,  0.0,  1.0 ),
        vec4( 1.0,  0.0,  0.0,  0.0 ),
        vec2( 0.5,  0.5             )
    ),
    ChunkVertexData(
        vec4( 1.0,  1.0,  0.0,  1.0 ),
        vec4( 1.0,  0.0,  0.0,  0.0 ),
        vec2( 0.5,  1.0             )
    ),
    ChunkVertexData(
        vec4( 1.0,  1.0,  1.0,  1.0 ),
        vec4( 1.0,  0.0,  0.0,  0.0 ),
        vec2( 0.25, 1.0             )
    ),

    /* behind */
    ChunkVertexData(
        vec4( 1.0,  0.0,  0.0,  1.0 ),
        vec4( 0.0 , 0.0, -1.0,  0.0 ),
        vec2( 0.5,  0.5             )
    ),
    ChunkVertexData(
        vec4( 0.0,  0.0,  0.0,  1.0 ),
        vec4( 0.0 , 0.0, -1.0,  0.0 ),
        vec2( 0.75, 0.5             )
    ),
    ChunkVertexData(
        vec4( 0.0,  1.0,  0.0,  1.0 ),
        vec4( 0.0 , 0.0, -1.0,  0.0 ),
        vec2( 0.75, 1.0             )
    ),
    ChunkVertexData(
        vec4( 1.0,  1.0,  0.0,  1.0 ),
        vec4( 0.0 , 0.0, -1.0,  0.0 ),
        vec2( 0.5,  1.0             )
    ),

    /* left */
    ChunkVertexData(
        vec4( 0.0,  0.0,  0.0,  1.0 ),
        vec4(-1.0,  0.0,  0.0,  0.0 ),
        vec2( 0.75, 0.5             )
    ),
    ChunkVertexData(
        vec4( 0.0,  0.0,  1.0,  1.0 ),
        vec4(-1.0,  0.0,  0.0,  0.0 ),
        vec2( 1.0,  0.5             )
    ),
    ChunkVertexData(
        vec4( 0.0,  1.0,  1.0,  1.0 ),
        vec4(-1.0,  0.0,  0.0,  0.0 ),
        vec2( 1.0,  1.0             )
    ),
    ChunkVertexData(
        vec4( 0.0,  1.0,  0.0,  1.0 ),
        vec4(-1.0,  0.0,  0.0,  0.0 ),
        vec2( 0.75, 1.0             )
    )
);
