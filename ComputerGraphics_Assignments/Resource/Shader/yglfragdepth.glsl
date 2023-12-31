#version 330 core

in vec4 vert_Color;
out vec4 out_Color;

void main()
{
	float t = (gl_FragCoord.z - 0.99f) * 100.0f;
	out_Color = mix(vert_Color, vec4(1.0f), t);
}