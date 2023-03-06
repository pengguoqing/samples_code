#version 330
layout (location = 0) in vec3 vertexpos;
layout (location = 1) in vec2 uvpos;

out vec2 texcoord;

void main(void)
{
    texcoord     = uvpos;
    gl_Position  = vec4(vertexpos, 1.f);
}