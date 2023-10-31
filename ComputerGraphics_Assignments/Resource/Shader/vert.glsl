#version 330 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Color;

uniform mat4 model_Transform;
uniform vec3 model_Color;

out vec3 vert_Color;

void main()
{
	gl_Position = model_Transform * vec4(in_Position, 1.0f);
	vert_Color = model_Color;
} 