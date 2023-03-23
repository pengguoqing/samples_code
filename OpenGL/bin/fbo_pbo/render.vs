#version 330 core

layout (location = 0) in vec3 vertexpos;
layout (location = 1) in vec2 uvpos;

out vec2 texcoord;

void main(void)
{
    texcoord     = vec2(uvpos.x, uvpos.y);
    gl_Position  = vec4(vertexpos, 1.f);
}