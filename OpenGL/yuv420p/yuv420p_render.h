#pragma once
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "shader_parse.hpp"
#include <vector>

class Yuv420pRender
{
public:
	Yuv420pRender();
	~Yuv420pRender();

	bool InitRender(int frame_w, int frame_h);
	bool UpLoadFrame(uint8_t* y, uint8_t* u, uint8_t* v);
	void RenderFrame();

private:
	bool InitShader();
	bool CreateTexture();

private:
    uint32_t m_tex_y;
	uint32_t m_tex_u;
	uint32_t m_tex_v;
	uint32_t m_vertex_array;
	uint32_t m_vertex_buffer;
	uint32_t m_index_buffer;
	const  int m_wnd_width;
	const  int m_wnd_height;

	int		 m_tex_width;
	int		 m_tex_height;

	ShaderParse m_shader_parse;
};

