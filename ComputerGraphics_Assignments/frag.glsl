#version 330 core

in vec3 vert_Color;
out vec4 out_Color;

void main()
{
	out_Color = vec4(vert_Color * (gl_FragCoord.z), 1.0f);
}