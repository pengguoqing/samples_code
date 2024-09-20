#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "shader_parse.hpp"
#include "glm/glm.hpp"

#include <vector>
#include <memory>

class YUV422Render
{
public:
	YUV422Render();
	~YUV422Render();

	bool InitRender(int frame_w, int frame_h);
	bool UpLoadFrame(uint8_t* yuyv);
	void RenderFrame();

private:
	bool InitShader();
	bool CreateTexture();

private:
	uint32_t m_tex_yuyv;
	uint32_t m_vertex_array;
	uint32_t m_vertex_buffer;
	uint32_t m_index_buffer;

	int		 m_tex_width;
	int		 m_tex_height;

	ShaderParse m_shader_parse;
};

