#version 460 core
out vec4 FragColor;
	
flat in uint color_idx;
in vec3 Normal;
	
uniform sampler2D palette;
uniform vec3 light_direction;
	
void main()
{         
	float idx = uintBitsToFloat(color_idx);
    vec3 texCol = texture(palette, vec2(idx / 256.0, 0.5)).rgb;      

	vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light_direction);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 result = diff * texCol;

    FragColor = vec4(texCol, 1.0f);
}