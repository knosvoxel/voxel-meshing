#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in uint color_index;

uniform mat4 mvp;
	
flat out uint color_idx;
	
void main()
{
    color_idx = color_index;
	gl_PointSize = 5;

    gl_Position = mvp * vec4(aPos, 1.0);
}