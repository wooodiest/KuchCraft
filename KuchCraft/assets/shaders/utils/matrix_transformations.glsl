mat4 TranslationMatrix(vec3 translation)
{
    return mat4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(translation  , 1.0)
    );
}

mat4 ScaleMatrix(vec3 scale)
{
    return mat4(
        vec4(scale.x, 0.0,      0.0,      0.0),
        vec4(0.0,     scale.y,  0.0,      0.0),
        vec4(0.0,     0.0,      scale.z,  0.0),
        vec4(0.0,     0.0,      0.0,      1.0)
    );
}

mat4 RotationMatrix(vec3 angles)
{
    float cosX = cos(angles.x);
    float sinX = sin(angles.x);
    float cosY = cos(angles.y);
    float sinY = sin(angles.y);
    float cosZ = cos(angles.z);
    float sinZ = sin(angles.z);

    mat4 rotationX = mat4( 1.0,  0.0,   0.0,  0.0,
                           0.0,  cosX, -sinX, 0.0,
                           0.0,  sinX,  cosX, 0.0,
                           0.0,  0.0,   0.0,  1.0);

    mat4 rotationY = mat4( cosY, 0.0,   sinY, 0.0,
                           0.0,  1.0,   0.0,  0.0,
                          -sinY, 0.0,   cosY, 0.0,
                           0.0,  0.0,   0.0,  1.0);

    mat4 rotationZ = mat4( cosZ, -sinZ, 0.0,  0.0,
                           sinZ,  cosZ, 0.0,  0.0,
                           0.0,   0.0,  1.0,  0.0,
                           0.0,   0.0,  0.0,  1.0);

    return rotationZ * rotationY * rotationX;
}