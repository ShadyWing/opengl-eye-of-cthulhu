#version 460 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D iScreenTexture;

const float offset = 1.0f/300.0f;

vec2 offsets[9] = vec2[]
(
	vec2(-offset,  offset), vec2( 0.0f,    offset), vec2( offset,  offset),
    vec2(-offset,  0.0f),   vec2( 0.0f,    0.0f),   vec2( offset,  0.0f),
    vec2(-offset, -offset), vec2( 0.0f,   -offset), vec2( offset, -offset)
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

void main()
{
    vec4 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = texture(iScreenTexture, TexCoord.st + offsets[i]);
    }
    vec4 col = vec4(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * sharpen[i];

    FragColor = col;
}