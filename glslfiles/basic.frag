// Fragment Shader

#version 150

in  vec3 ex_Color;  //colour arriving from the vertex
out vec4 out_Color; //colour for the pixel

void main(void)
{
	out_Color = vec4(ex_Color,1.0);
}