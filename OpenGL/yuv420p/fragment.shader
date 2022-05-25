#version 330 core

out vec4 frage_color;

in vec2 tex_uv;

uniform sampler2D tex_y;
uniform sampler2D tex_u;
uniform sampler2D tex_v;

void main()
{
	vec3 yuv = vec3(0.f);
    
    yuv.x = texture2D(tex_y, tex_uv).r - 16.f/235.f;
    yuv.y = texture2D(tex_u, tex_uv).r - 128.f/240.f;
    yuv.z = texture2D(tex_v, tex_uv).r - 128.f/240.f;
   
    yuv = clamp(yuv, 0.f, 1.f);
    
    //709_YUV_to_RGB
	/*mat3 yuv_to_rgb = mat3(1.164f,  0.f,     1.793f,
	                       1.164f, -0.213f, -0.533f,
	                       1.164f,  2.112f,  0.f);*/
    
    mat3 yuv_to_rgb = mat3(1.164f, 1.164f,  1.164f,
                           0.f,    -0.213f, 2.112f,
                           1.793f, -0.533f, 0.f);

    vec3 rgb = yuv_to_rgb *yuv;
    frage_color = vec4(rgb, 1.f);
}