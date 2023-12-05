#version 460 core

uniform vec3 light_Position;
uniform vec3 view_Position;

uniform vec3 ambient_Color;
uniform vec3 diffuse_Color;
uniform vec3 specular_Color;

in vec3 vert_Position;
in vec4 vert_Color;
in vec3 vert_Normal;
in vec2 vert_UV;

out vec4 out_Color;

void main()
{
	vec3 normal = normalize(vert_Normal);
	vec3 light_Direction = normalize(light_Position - vert_Position);
	vec3 view_Direction = normalize(view_Position - vert_Position);

	float ambient_Light = 0.25;
	vec3 ambient = ambient_Color * ambient_Light;

	float diffuse_Light = max(dot(normal, light_Direction), 0.0);
	vec3 diffuse = diffuse_Color * diffuse_Light;

	float shiny = 256.0;
	vec3 reflect_Direction = reflect(-light_Direction, normal);
	float specular_Light = max(0.0, dot(view_Direction, reflect_Direction));
	specular_Light = pow(specular_Light, shiny);
	vec3 specular = specular_Color * specular_Light;

	out_Color = vec4(ambient + diffuse + specular, 0.5) * (vert_Color + vec4(vert_Position, 0.0));
}