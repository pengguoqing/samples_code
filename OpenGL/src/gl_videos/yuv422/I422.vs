#version 330 core
layout(location = 0) in vec3 vertex_pos;
layout(location = 1) in vec2 tex_pos;

out vec2 tex_uv;

void main()
{
	gl_Position = vec4(vertex_pos, 1.f);
	tex_uv = vec2(tex_pos);
}