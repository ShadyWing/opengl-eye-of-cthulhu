#version 460 core

struct Material{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	sampler2D texture_height1;
	sampler2D texture_alpha1;
	float shininess;
};

struct DirectionalLight{
					//基准	偏移(必须是基准的倍数
	vec3 direction;	//16	16

	vec3 ambient;	//16	32
	vec3 diffuse;	//16	48
	vec3 specular;	//16	64

	float constant;	//4		80
	float linear;	//4		84
	float quadratic;//4		88
};

struct PointLight{
					//基准	偏移(必须是基准的倍数
	vec3 position;	//16	96

	vec3 ambient;	//16	112
	vec3 diffuse;	//16	128
	vec3 specular;	//16	144

	float constant;	//4		160
	float linear;	//4		164
	float quadratic;//4		168
};

struct SpotLight{
	vec3 position;
	vec3 direction;
	// 光照范围角 余弦
	float cutOff;
	// 柔边
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform Material material;
uniform vec3 viewPos;
uniform PointLight pointLight;
uniform DirectionalLight dirLight;

vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);

	vec3 ambient  = light.ambient * texture(material.texture_diffuse1, TexCoord).rgb;

	float diff = max(dot(lightDir,normal),0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1,TexCoord).rgb;

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * texture(material.texture_specular1, TexCoord).rgb;

	return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);

	vec3 ambient  = light.ambient * texture(material.texture_diffuse1, TexCoord).rgb;

	float diff = max(dot(lightDir,normal),0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1,TexCoord).rgb;

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * texture(material.texture_specular1, TexCoord).rgb;

	float distance = length(light.position - fragPos);
	float intensity = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	return (intensity * (ambient + diffuse + specular));
}

void main()
{
	vec3 result;
	vec3 normal = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	
	result = calcDirectionalLight(dirLight, normal, FragPos, viewDir);
	result += calcPointLight(pointLight, normal, FragPos, viewDir);

	float alpha = texture(material.texture_alpha1, TexCoord).g;
	if(alpha < 0.1)
		discard;

	FragColor = vec4(result, alpha);
	//FragColor = vec4(vec3(gl_FragCoord.z), alpha);
}