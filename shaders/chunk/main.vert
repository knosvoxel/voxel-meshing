#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

//out vec3 ourColor;
out vec3 Normal;
out vec3 Color;

uniform mat4 mvp;

void main()
{
    Normal = aNormal;
    Color = aColor;

    gl_Position = mvp * vec4(aPos, 1.0);
}