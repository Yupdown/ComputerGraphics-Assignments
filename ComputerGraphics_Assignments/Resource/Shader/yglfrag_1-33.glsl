#version 460 core

uniform vec3 light_Position;
uniform vec3 view_Position;

in vec3 vert_Position;
in vec4 vert_Color;
in vec3 vert_Normal;
in vec2 vert_UV;

out vec4 out_Color;

void main()
{
	vec3 normal = normalize(vert_Normal);
	out_Color = vec4(normal, 0.5f);
}