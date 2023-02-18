#version 330 core

out vec4 frage_color1;

in vec2 tex_uv;

uniform sampler2D tex_yuyv;


uniform vec4 color_vec0;
uniform vec4 color_vec1;
uniform vec4 color_vec2;

uniform vec3 color_range_min;
uniform vec3 color_range_max;

uniform int rt_width;


void main()
{

    vec4 yuyv = texture2D(tex_yuyv, tex_uv);
    vec2 yy = yuyv.xz;
    vec2 cbcr = yuyv.yw;

    vec2 tex_size = textureSize(tex_yuyv, 0);
    float factor = tex_size.x/rt_width;
    float cur_y = (tex_uv.x * factor) < 0.5f ? yy.x : yy.y;
    
    vec3 yuv = vec3(cur_y, cbcr);
    yuv = clamp(yuv, color_range_min, color_range_max);

    vec3 rgb = vec3(0.f);
    rgb.r = dot(color_vec0.xyz, yuv) + color_vec0.w;
	rgb.g = dot(color_vec1.xyz, yuv) + color_vec1.w;
	rgb.b = dot(color_vec2.xyz, yuv) + color_vec2.w;

    frage_color1 = vec4(rgb, 1.f);
}