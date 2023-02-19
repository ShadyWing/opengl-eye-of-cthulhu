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

float kernel[9] = float[]
(
	1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
	2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
	1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0
);


const float Pi = 6.28318530718; // Pi*2
// GAUSSIAN BLUR SETTINGS
const float Directions = 12.0; // BLUR DIRECTIONS (Default 16.0 - More is better but slower)
const float Quality = 4.0;     // BLUR QUALITY (Default 4.0 - More is better but slower)
const float Radius = 0.03;     // BLUR SIZE (Radius)

void main()
{
	// Normalized pixel coordinates (from 0 to 1)
	// Pixel colour
	vec4 Color = texture(iScreenTexture, TexCoord);
	
	// Blur calculations
	for( float d=0.0; d<Pi; d+=Pi/Directions)
	{
		for(float i=1.0/Quality; i<=1.0; i+=1.0/Quality)
		{
			Color += texture(iScreenTexture, TexCoord+vec2(cos(d),sin(d))*Radius*i);		
		}
	}
	
	// Output to screen
	Color /= (Quality * Directions -11.0);

	//if(Color.a < 0.01f)
	//	discard;

	FragColor = Color;
}