#version 330 core
layout (location = 0) in vec3 vertexpos;
layout (location = 1) in vec2 texcoord;

out vec2 texcoord;

void main(void)
{
	gl_Position = vec4(vertexpos, 1.0);
	texcoord    = texcoord;
}