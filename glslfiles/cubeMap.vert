#version 150

in vec3 in_Position;
out vec3 TexCoords;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main()
{
    gl_Position = ProjectionMatrix * ViewMatrix * vec4(in_Position, 1.0);
    TexCoords = in_Position;
} 