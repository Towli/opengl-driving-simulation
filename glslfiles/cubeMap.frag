#version 150

in vec3 TexCoords;
out vec4 out_Color;

uniform samplerCube skybox;

void main()
{    
    out_Color = texture(skybox, TexCoords);
}
  