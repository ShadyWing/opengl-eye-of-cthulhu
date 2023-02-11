#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoord;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	vec4 pos = projection * view * vec4(aPos, 1.0f);
	// 使天空盒 → 天空球，gl_Position是屏幕空间坐标，xyww, 代表深度为1
	gl_Position = pos.xyww;
	TexCoord = aPos;
}