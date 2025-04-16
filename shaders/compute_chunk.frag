#version 460 core
out vec4 FragColor;
	
flat in uint color_idx;
	
uniform sampler2D palette;
	
void main()
{         
	float idx = uintBitsToFloat(color_idx);
    vec3 texCol = texture(palette, vec2(idx / 256.0, 0.5)).rgb;      
    FragColor = vec4(texCol, 1.0);
}