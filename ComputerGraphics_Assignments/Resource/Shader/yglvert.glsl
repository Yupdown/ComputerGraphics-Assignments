#version 460 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Color;
layout(location = 2) in vec3 in_Normal;
layout(location = 3) in vec3 in_UV;

layout(std140, binding = 0) uniform GlobalUniform
{
	mat4 view_Transform;
	mat4 proj_Transform;
};

uniform mat4 model_Transform;

out vec3 vert_Color;

void main()
{
	gl_Position = proj_Transform * view_Transform * model_Transform * vec4(in_Position, 1.0f);
	vert_Color = in_Normal;
} 