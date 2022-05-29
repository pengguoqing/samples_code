#version 330 core

out vec4 frage_color;

in vec2 tex_uv;
uniform sampler2D tex_yuyv;
vec3 yuv0;
vec3 yuv1;

void main()
{
	vec2 tex_coord = tex_uv;

	//转RGB用的 YUV
	vec3 yuv = vec3(0.f);

	//纹理采样后的YUYV
	vec4 yuyv = vec4(0.f);

	ivec2 texture_size = textureSize(tex_yuyv, 0);
	int pos_x = int(gl_FragCoord.x * texture_size.x * 2.0f);
   
	if (0 == mod(pos_x, 2.f))
	{
		yuyv = texture2D(tex_yuyv, tex_coord);
		yuv0.x = yuyv.x;
		yuv0.y = yuyv.y;
		yuv0.z = yuyv.w;

		yuv1.x = yuyv.z;
		yuv1.y = yuyv.y;
		yuv1.z = yuyv.w;

		yuv = yuv0;
	}
	else
	{
	    yuv = yuv1;	
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