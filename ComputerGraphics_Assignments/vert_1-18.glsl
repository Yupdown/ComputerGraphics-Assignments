#version 330

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Color;

uniform mat4 model_Transform;
uniform mat4 world_Transform;

out vec3 vert_Color;

void main()
{
	gl_Position = model_Transform * world_Transform * vec4(in_Position, 1.0f);
	vert_Color = in_Color * vec3(world_Transform * vec4(in_Position, 1.0f)) + 0.5f;
} 