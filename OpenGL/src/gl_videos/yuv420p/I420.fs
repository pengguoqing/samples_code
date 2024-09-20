#version 330 core

out vec4 frage_color;

in vec2 tex_uv;

uniform sampler2D tex_y;
uniform sampler2D tex_u;
uniform sampler2D tex_v;

uniform vec4 color_vec0;
uniform vec4 color_vec1;
uniform vec4 color_vec2;

uniform vec3 color_range_min;
uniform vec3 color_range_max;

void main()
{
	vec3 yuv = vec3(0.f);
    
    yuv.x = texture2D(tex_y, tex_uv).r;
    yuv.y = texture2D(tex_u, tex_uv).r;
    yuv.z = texture2D(tex_v, tex_uv).r;
 
    yuv = clamp(yuv, color_range_min, color_range_max);

    vec3 rgb = vec3(0.f);
    rgb.r = dot(color_vec0.xyz, yuv) + color_vec0.w;
	rgb.g = dot(color_vec1.xyz, yuv) + color_vec1.w;
	rgb.b = dot(color_vec2.xyz, yuv) + color_vec2.w;

    frage_color = vec4(rgb, 1.f);
}