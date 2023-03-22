#version 330 core

layout (location = 0) in vec3 vertexpos;
layout (location = 1) in vec2 uvpos;

out vec2 texcoord;

uniform float width_offset;

void main()
{
    gl_Position  = vec4(vertexpos, 1.f);
    texcoord     = vec2(uvpos.x+width_offset, uvpos.y);
}