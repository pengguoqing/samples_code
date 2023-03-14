#version 330

out vec3 fragcolor;
in vec2 texcoord;

uniform sampler2D image;

uniform vec3 color_range_min;
uniform vec3 color_range_max;
uniform vec4 color_vec0;
uniform vec4 color_vec1;
uniform vec4 color_vec2;

vec3 YUV_to_RGB(vec3 yuv)
{
	yuv = clamp(yuv, color_range_min, color_range_max);
	float r = dot(color_vec0.xyz, yuv) + color_vec0.w;
	float g = dot(color_vec1.xyz, yuv) + color_vec1.w;
	float b = dot(color_vec2.xyz, yuv) + color_vec2.w;
	return vec3(r, g, b);
}

vec3 SampleYUYV(vec2 pos)
{
	vec2 size = textureSize(image, 0);
	vec2 p = (vec2(p_lod.xy) + 0.5) / size;
	vec4 color = textureLod(s, p, lod);
	return color;

}

void main(void)
{
    fragcolor = vec3(1.f, 1.f, 1.f);
}