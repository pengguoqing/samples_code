#version 330 core

out vec4 screencolor;

in vec2 texcoord;

uniform sampler2D rendertex;

void main()
{ 
    screencolor = texture(rendertex, texcoord);

    //screencolor = vec4(1.f, 1.f, 1.f, 1.f);
} 