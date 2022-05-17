#version 330 core

out vec4 frage_color;

in vec2 tex_uv;
//uniform sampler2D tex_y;
//uniform sampler2D tex_u;
//uniform sampler2D tex_v;

void main()
{
	//vec3 yuv = vec3(0.f);
 //   vec3 rgb = vec3(0.f);
 //   yuv.x = texture(tex_y, tex_uv).r - 16.f/235.f;
 //   yuv.y = texture(tex_u, tex_uv).r - 128.f/240.f;
 //   yuv.z = texture(tex_v, tex_uv).r - 128.f/240.f;
 //   rgb = mat3(1.164f, 0.f,      1.f,
 //              1.164f, -0.213f, -0.533f,
 //              1.164f, 2.112f,   0.f) * yuv;
    
    frage_color = vec4(1.f, 0.f, 0.f, 1.f);
	
}
