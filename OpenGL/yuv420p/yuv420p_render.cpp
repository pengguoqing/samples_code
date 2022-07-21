#include "yuv420p_render.h"


static const glm::vec4 color_vec0(1.16438353f, 0.00000000f,  1.79274106f, -0.972945154f);
static const glm::vec4 color_vec1(1.16438353f, -0.213248610f, -0.532909334f, 0.301482677f);
static const glm::vec4 color_vec2(1.16438353f, 2.11240172f,  0.f, -1.13340223f);

static const glm::vec3 color_range_min(0.0627451017f, 0.0627451017f, 0.0627451017f);
static const glm::vec3 color_range_max(0.921568632f, 0.941176474f, 0.941176474f);

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
	glDeleteTextures(1, &m_tex_y);
	glDeleteTextures(1, &m_tex_u);
	glDeleteTextures(1, &m_tex_v);
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

	m_shader_parse.use();
	m_shader_parse.setVec3("color_range_min", color_range_min);
	GLenum errorcode = glGetError();
	m_shader_parse.setVec3("color_range_max", color_range_max);
	errorcode = glGetError();
	m_shader_parse.setVec4("color_vec0", color_vec0);
	errorcode = glGetError();
	m_shader_parse.setVec4("color_vec1", color_vec1);
	errorcode = glGetError();
	m_shader_parse.setVec4("color_vec2", color_vec2);
	errorcode = glGetError();
	glBindVertexArray(m_vertex_array);
	
	return true;
}

bool Yuv420pRender::UpLoadFrame(uint8_t* y, uint8_t* u, uint8_t* v)
{

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_tex_y);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_tex_width, m_tex_height, GL_RED, GL_UNSIGNED_BYTE, y);
	m_shader_parse.setInt("tex_y", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_tex_u);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_tex_width/2, m_tex_height/2, GL_RED, GL_UNSIGNED_BYTE, u);
	m_shader_parse.setInt("tex_u", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_tex_v);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_tex_width/2, m_tex_height/2, GL_RED, GL_UNSIGNED_BYTE, v);
	m_shader_parse.setInt("tex_v", 2);
	
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
	m_shader_parse.InitShader("vertex.shader", "I420.fs");


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

	glBindVertexArray(m_vertex_array);

	return true;
}

bool Yuv420pRender::CreateTexture()
{
	glGenTextures(1, &m_tex_y);
	glBindTexture(GL_TEXTURE_2D, m_tex_y);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_tex_width, m_tex_height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glGenerateMipmap(GL_TEXTURE_2D);
	m_shader_parse.setInt("tex_y", 0);
	

	glGenTextures(1, &m_tex_u);
	glBindTexture(GL_TEXTURE_2D, m_tex_u);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_tex_width/2, m_tex_height/2, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glGenerateMipmap(GL_TEXTURE_2D);
	m_shader_parse.setInt("tex_v", 1);

	glGenTextures(1, &m_tex_v);
	glBindTexture(GL_TEXTURE_2D, m_tex_v);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_tex_width/2, m_tex_height/2, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glGenerateMipmap(GL_TEXTURE_2D);
	m_shader_parse.setInt("tex_v", 2);

	return true;
}


