#version 150

in vec3 TexCoords;
out vec4 out_Color;

uniform vec4 Brightness;
uniform samplerCube skybox;

void main()
{    
	out_Color = Brightness * texture(skybox, TexCoords);
}