#version 330 core

out vec4 frage_color;

in vec2 tex_uv;
uniform sampler2D tex_yuyv;

void main()
{
	vec2 tex_coord = tex_uv;

	//转RGB用的 YUV
	vec3 yuv = vec3(0.f);

	//纹理采样后的YUYV
	vec4 yuyv = vec4(0.f);

	ivec2 texture_size = textureSize(tex_yuyv, 0);
	int pos_x = int(texture_size.x * tex_coord.x);
    
	//因为是YUYV排列, 所以采样第 1, 3, 5, 7...时需要采样前一个像素位置
	if (0 == mod(pos_x, 2.f))
	{
		yuyv = texture2D(tex_yuyv, vec2(tex_coord.x, tex_coord.y));
		yuv.x = yuyv.x;
		yuv.y = yuyv.y;
		yuv.z = yuyv.w;
	}
	else
	{
		tex_coord.x -= 1.f/ texture_size.x;
		yuyv = texture2D(tex_yuyv, vec2(tex_coord.x, tex_coord.y));
		yuv.x = yuyv.z;
		yuv.y = yuyv.y;
		yuv.z = yuyv.w;
	}

	yuv.x -= 16.f / 235.f;
	yuv.y -= 128.f / 240.f;
	yuv.z -= 128.f / 240.f;

	yuv = clamp(yuv, 0.f, 1.f);

	mat3 yuv_to_rgb = mat3(1.164f, 1.164f, 1.164f,
						   0.f, -0.213f, 2.112f,
						   1.793f, -0.533f, 0.f);

	vec3 rgb = yuv_to_rgb * yuv;
	frage_color = vec4(rgb, 1.f);
}