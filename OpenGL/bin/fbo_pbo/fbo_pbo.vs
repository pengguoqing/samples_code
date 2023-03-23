#version 330 core

layout (location = 0) in vec3 vertexpos;
layout (location = 1) in vec2 uvpos;

out vec2 texcoord;

uniform float width_offset;
uniform float width_d2;
uniform float height;

void main()
{
    gl_Position  = vec4(vertexpos, 1.f);
    float  u     = (uvpos.x+width_offset) * width_d2; 
    float  v     = uvpos.y * height;
    texcoord     = vec2(u, v);
}