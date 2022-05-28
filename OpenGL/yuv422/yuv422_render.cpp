#include "yuv422_render.h"

YUV422Render::YUV422Render()
    :m_tex_yuyv(-1),
     m_index_buffer(-1),
     m_tex_width(-1),
     m_tex_height(-1),
     m_vertex_array(-1),
     m_vertex_buffer(-1)
{

}

YUV422Render::~YUV422Render()
{
	glDeleteVertexArrays(1, &m_vertex_array);
	glDeleteBuffers(1, &m_vertex_buffer);
	glDeleteBuffers(1, &m_index_buffer);
    glDeleteTextures(1, &m_tex_yuyv);
	glfwTerminate();
}

bool YUV422Render::InitRender(int frame_w, int frame_h)
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

	m_shader_parse.use();
	glBindVertexArray(m_vertex_array);

	return true;

    return false;
}

bool YUV422Render::UpLoadFrame(uint8_t* yuyv)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_tex_yuyv);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_tex_width/2 , m_tex_height, GL_RGBA, GL_UNSIGNED_BYTE, yuyv);
	m_shader_parse.setInt("tex_yuyv", 0);
    
	return true;
}

void YUV422Render::RenderFrame()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_tex_yuyv);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	return;
}

bool YUV422Render::InitShader()
{
	m_shader_parse.InitShader("yuyv_vertex.shader", "yuyv_fragment.shader");

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

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(m_vertex_array);

	return true;
}

bool YUV422Render::CreateTexture()
{
	glGenTextures(1, &m_tex_yuyv);
	glBindTexture(GL_TEXTURE_2D, m_tex_yuyv);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_tex_width/2, m_tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glGenerateMipmap(GL_TEXTURE_2D);
	m_shader_parse.setInt("tex_yuyv", 0);
	
	return true;
}
