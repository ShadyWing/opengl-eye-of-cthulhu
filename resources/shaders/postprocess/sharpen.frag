#version 460 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D iScreenTexture;

const float offsetw = 1.0f / 1280.0f * 4;
const float offseth = 1.0f / 720.0f * 4;

vec2 offsets[9] = vec2[]
(
	vec2(-offsetw,  offseth), vec2( 0.0f,    offseth), vec2( offsetw,  offseth),
	vec2(-offsetw,  0.0f),    vec2( 0.0f,    0.0f),    vec2( offsetw,  0.0f),
	vec2(-offsetw, -offseth), vec2( 0.0f,   -offseth), vec2( offsetw, -offseth)
);

float sharpen[9] = float[]
(
	-1, -1, -1,
	-1,  9, -1,
	-1, -1, -1
);

float blur[9] = float[]
(
	1.0 / 16, 2.0 / 16, 1.0 / 16,
	2.0 / 16, 4.0 / 16, 2.0 / 16,
	1.0 / 16, 2.0 / 16, 1.0 / 16  
);

float edge[9] = float[]
(
	 1,  1,  1,
	 1, -8,  1,
	 1,  1,  1
);

float noeffect[9] = float[]
(
	 0,  0,  0,
	 0,  1,  0,
	 0,  0,  0
);

void main()
{
	vec4 sampleTex[9];
	for(int i = 0; i < 9; i++)
	{
		sampleTex[i] = texture(iScreenTexture, TexCoord.st + offsets[i]);
	}
	vec4 col = vec4(0.0);
	for(int i = 0; i < 9; i++)
		col += sampleTex[i] * noeffect[i];

	FragColor = col;
}