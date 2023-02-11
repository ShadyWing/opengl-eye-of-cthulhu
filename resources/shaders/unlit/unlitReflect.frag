#version 460 core

struct UnlitMaterial{
	vec3 baseColor;
	sampler2D texture_diffuse1;
	sampler2D texture_height1;
	sampler2D texture_alpha1;
};

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform UnlitMaterial material;
uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{
	float ratio = 1.00 / 1.52;
	vec3 I = normalize(FragPos - cameraPos);
	//vec3 R = reflect(I, normalize(Normal));
	vec3 R = refract(I, normalize(Normal), ratio);
	FragColor = vec4(texture(skybox, R).rgb, 1.0f);
}