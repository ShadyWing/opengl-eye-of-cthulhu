#version 460 core

struct UnlitMaterial{
	vec3 baseColor;
	sampler2D texture_diffuse1;
	sampler2D texture_height1;
	sampler2D texture_alpha1;
};

out vec4 FragColor;

in vec2 TexCoord;

uniform UnlitMaterial material;
uniform float diffuseOpacity;

void main()
{
	FragColor = vec4(material.baseColor, 1.0f);
}