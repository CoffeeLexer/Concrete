#version 330

mat3x2 positions = mat3x2(
    0.0,  0.5,
    0.5, -0.5,
   -0.5, -0.5
);

void main()
{
    vec2 position = positions[gl_VertexIndex];
    gl_Position = vec4(position, 0.0, 1.0);
}
