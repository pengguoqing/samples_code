#pragma once
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <vector>

class Yuv420pRender
{
public:
	Yuv420pRender();
	~Yuv420pRender();

	bool InitRender(int frame_w, int frame_h);
	bool UpLoadFrame(const std::vector<uint8_t*> frame_data);
	void RenderFrame();

private:
	bool InitEnviroment();
	bool InitShader();
	bool CreateTexture();

	static void window_size_callback(GLFWwindow* win, int width, int height);

private:
    uint32_t m_tex_y;
	uint32_t m_tex_u;
	uint32_t m_tex_v;
	uint32_t m_shader_pro;
	uint32_t m_vertex_array;
	GLFWwindow* m_wnd;
	const  int m_wnd_width;
	const  int m_wnd_height;

	int		 m_tex_width;
	int		 m_tex_height;
};

