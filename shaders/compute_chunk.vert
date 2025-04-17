#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in uint color_index;

uniform mat4 mvp;
	
flat out uint color_idx;
out vec3 Normal;
	
void main()
{
    color_idx = color_index;
	Normal = aNormal;

    gl_Position = mvp * vec4(aPos, 1.0);
//	gl_PointSize = 5;

}