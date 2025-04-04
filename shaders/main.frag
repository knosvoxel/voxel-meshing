#version 460 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Color;

uniform vec3 light_direction;
uniform vec3 color;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light_direction);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 result = diff * Color;

    FragColor = vec4(result, 1.0f);
}