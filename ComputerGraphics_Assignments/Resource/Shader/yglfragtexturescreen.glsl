#version 460 core

uniform sampler2D main_Texture;
uniform vec3 view_Position;

in vec3 vert_Position;
in vec4 vert_Color;
in vec3 vert_Normal;
in vec2 vert_UV;

out vec4 out_Color;

void main()
{
	out_Color = texture(main_Texture, gl_Position.xy);
}