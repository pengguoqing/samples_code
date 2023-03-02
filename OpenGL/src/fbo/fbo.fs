#version 330 core

out  vec4 fragColor;

in   vec2 texcoord;

uniform sampler2D image;

void main(void)
{
    fragColor = texture(image, texcoord);
}
