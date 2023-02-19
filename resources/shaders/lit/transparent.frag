#version 460 core

struct Material{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	sampler2D texture_height1;
	sampler2D texture_alpha1;
	float shininess;
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

struct Light{
	bool isEnabled;	
	bool isLocal;	// true 点光或锥光，false 方向光
	bool isSpot;

	vec3 ambient;
	vec3 color;

	vec3 position;
	vec3 direction;
	float spotCosCutoff;
	float spotExponent;
	// 衰弱
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
const int LIGHT_NUM = 16;
uniform Light lights[LIGHT_NUM];

vec3 calcDirectionalLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);

	vec3 ambient  = light.ambient * texture(material.texture_diffuse1, TexCoord).rgb;

	float diff = max(dot(lightDir,normal),0.0);
	vec3 diffuse = light.color * diff * texture(material.texture_diffuse1,TexCoord).rgb;

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.color * spec * texture(material.texture_specular1, TexCoord).rgb;

	return (ambient + diffuse + specular);
}

vec3 calcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);

	vec3 ambient  = light.ambient * texture(material.texture_diffuse1, TexCoord).rgb;

	float diff = max(dot(lightDir,normal),0.0);
	vec3 diffuse = light.color * diff * texture(material.texture_diffuse1,TexCoord).rgb;

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.color * spec * texture(material.texture_specular1, TexCoord).rgb;

	float distance = length(light.position - fragPos);
	float intensity = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	return (intensity * (ambient + diffuse + specular));
}

void main()
{
	vec3 result;
	vec3 normal = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	for(int i = 0; i < LIGHT_NUM; i++)
	{
		if(lights[i].isEnabled)
		{
			if(lights[i].isLocal)
				result += calcPointLight(lights[i], normal, FragPos, viewDir);
			else
				result += calcDirectionalLight(lights[i], normal, FragPos, viewDir);
		}
	}

	float alpha = texture(material.texture_alpha1, TexCoord).g;
	if(alpha < 0.1)
		discard;

	FragColor = vec4(result, alpha);
	//FragColor = vec4(vec3(gl_FragCoord.z), alpha);
}