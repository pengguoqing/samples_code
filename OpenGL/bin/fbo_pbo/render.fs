#version 330 core
out vec4 FragColor;

in vec2 texcoord;

uniform sampler2D rendertex;

void main()
{
    //FragColor = texture(rendertex, texcoord);
    FragColor = vec4(1.f, 1.f, 1.f, 1.f);
} 