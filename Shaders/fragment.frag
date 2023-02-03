#version 460 core

struct Material{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	float shininess;
};

struct Light{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
	vec3 ambient = light.ambient * texture(material.diffuse,TexCoord).rgb;

	vec3 norm = normalize(Normal);
	//vec3 norm = normalize(texture(material.normal, TexCoord).rgb * 2.0 - 1.0);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(lightDir,norm),0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse,TexCoord).rgb;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * texture(material.specular, TexCoord).rgb;

	FragColor = vec4(ambient + diffuse + specular, 1.0);
}