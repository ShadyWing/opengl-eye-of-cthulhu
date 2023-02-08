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
	//vec3 textureColor = texture(material.texture_diffuse1, TexCoord).rgb;
	//vec3 color;
	//
	//if(textureColor.r == textureColor.g && textureColor.r == textureColor.b && textureColor.r < 0.01f)
	//	color = material.baseColor;
	//else
		//color = mix(textureColor, material.baseColor, diffuseOpacity);

	FragColor = vec4(material.baseColor, 1.0f);
}