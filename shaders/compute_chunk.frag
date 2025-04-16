#version 460 core
out vec4 FragColor;
	
//in vec2 TexCoords;
flat in uint color_idx;
	
uniform sampler2D tex;
	
void main()
{             
    vec4 color = texture(tex, vec2(color_idx / 256.0, 0.5));

	FragColor = color;
}