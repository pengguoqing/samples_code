#include "yuv420p_render.h"

Yuv420pRender::Yuv420pRender()
	:m_wnd_width(1280),
	 m_wnd_height(720),
	 m_tex_width(-1),
	 m_tex_height(-1),
	 m_tex_y(-1),
	 m_tex_u(-1),
	 m_tex_v(-1),
	 m_vertex_array(-1),
	 m_vertex_buffer(-1),
	 m_index_buffer(-1)
{

}

Yuv420pRender::~Yuv420pRender()
{
	glDeleteVertexArrays(1, &m_vertex_array);
	glDeleteBuffers(1, &m_vertex_buffer);
	glDeleteBuffers(1, &m_index_buffer);
	glfwTerminate();
}

bool Yuv420pRender::InitRender(int frame_w, int frame_h)
{
	
	if (!InitShader())
	{
		return false;
	}

	m_tex_width = frame_w;
	m_tex_height = frame_h;
	if (!CreateTexture())
	{
		return false;
	}
	glBindVertexArray(m_vertex_array);
	m_shader_parse.use();
	return true;
}

bool Yuv420pRender::UpLoadFrame(uint8_t* y, uint8_t* u, uint8_t* v)
{
	glBindTexture(GL_TEXTURE_2D, m_tex_y);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_tex_width, m_tex_height, GL_RED, GL_UNSIGNED_BYTE, y);
	
	glBindTexture(GL_TEXTURE_2D, m_tex_u);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_tex_width/2, m_tex_height/2, GL_RED, GL_UNSIGNED_BYTE, u);
	
	glBindTexture(GL_TEXTURE_2D, m_tex_v);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_tex_width/2, m_tex_height/2, GL_RED, GL_UNSIGNED_BYTE, v);
	
	return true;
}

void Yuv420pRender::RenderFrame()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_tex_y);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_tex_u);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_tex_v);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	
	return ;
}

bool Yuv420pRender::InitShader()
{
	m_shader_parse.InitShader("vertex.shader", "fragment.shader");

	float vertex_coord_data[] = {
		-1.f, -1.f, 0.f,   0.f, 1.f,
		-1.f,  1.f, 0.f,   0.f, 0.f,
		 1.f,  1.f, 0.f,   1.f, 0.f,
		 1.f, -1.f, 0.f,   1.f, 1.f,
	};

	uint32_t vertx_index_data[] = {
		0, 1, 2,
		2, 3, 0
	};

	uint32_t m_vertex_buffer, m_index_buffer;
	glGenBuffers(1, &m_vertex_buffer);
	glGenBuffers(1, &m_index_buffer);

	glGenVertexArrays(1, &m_vertex_array);
	glBindVertexArray(m_vertex_array);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_coord_data), vertex_coord_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertx_index_data), vertx_index_data, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	return true;
}

bool Yuv420pRender::CreateTexture()
{
	glGenTextures(1, &m_tex_y);
	glBindTexture(GL_TEXTURE_2D, m_tex_y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_tex_width, m_wnd_height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glGenerateMipmap(GL_TEXTURE_2D);

	glGenTextures(1, &m_tex_u);
	glBindTexture(GL_TEXTURE_2D, m_tex_u);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_tex_width/2, m_wnd_height/2, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glGenerateMipmap(GL_TEXTURE_2D);

	glGenTextures(1, &m_tex_v);
	glBindTexture(GL_TEXTURE_2D, m_tex_v);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_tex_width/2, m_wnd_height/2, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glGenerateMipmap(GL_TEXTURE_2D);

	m_shader_parse.setInt("tex_y", 0);
	m_shader_parse.setInt("tex_u", 1);
	m_shader_parse.setInt("tex_v", 2);

	return true;
}


