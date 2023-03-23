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
	ivec2 size		= textureSize(image,  0);
	ivec2 actualpos = ivec2(pos.x, pos.y);
	vec2  actualuv	= (vec2(actualpos.xy)+0.5)/size;
	vec4 yuyv		= textureLod(image, actualuv, 0);
	
	float leftover  = fract(pos.x);
	float y   = (leftover<0.5f) ? yuyv.x : yuyv.z;

    vec3 yuv = vec3(y, yuyv.yw);

	return YUV_to_RGB(yuv);	
}

void main(void)
{
    //fragcolor = vec3(1.f, 1.f, 1.f);
	fragcolor = SampleYUYV(texcoord);
}