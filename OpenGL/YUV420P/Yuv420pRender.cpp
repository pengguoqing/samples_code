#include "Yuv420pRender.h"

const char* vertex_shader = 
"#version 330 core"
"layout(location = 0) in vec3 vertex_pos;"
"layout(location = 1) in vec2 texture_coord;"
"out vec2 tex_coord;"
"void main()\n"
"{"
    "gl_Position = vec4(vertex_pos.x, vertex_pos.y, vertex_pos.z, 0.0);"
    "tex_coord = vec2(texture_coord.x, texture_coord.y);"
"}";

const char* frage_shader = 
"#version 330 core"

"out vec4 frage_color;"

"in vec2 tex_coord;"
"uniform sampler2D tex_y; "
"uniform sampler2D tex_u;"
"uniform sampler2D tex_v;"
"void main()"
"{"
     
	"frage_color = vec4(0.f, 1.f, 0.f, 1.f);"
"}";

Yuv420pRender::Yuv420pRender()
	:m_wnd_width(1280),
	 m_wnd_height(720),
	 m_tex_width(-1),
	 m_tex_height(-1),
	 m_tex_y(-1),
	 m_tex_u(-1),
	 m_tex_v(-1),
	 m_shader_pro(-1),
	 m_vertex_array(-1),
	 m_wnd(nullptr)
{

}

Yuv420pRender::~Yuv420pRender()
{
	glDeleteVertexArrays(1, &m_vertex_array);
	glfwTerminate();
}

bool Yuv420pRender::InitRender(int frame_w, int frame_h)
{
	if (!InitEnviroment())
	{
		return false;
	}
	
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

	return true;
}

bool Yuv420pRender::UpLoadFrame(const std::vector<uint8_t*> frame_data)
{
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_tex_width, m_tex_height, GL_RED, GL_UNSIGNED_BYTE, frame_data[0]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_tex_width/2, m_tex_height/2, GL_RED, GL_UNSIGNED_BYTE, frame_data[1]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_tex_width/2, m_tex_height/2, GL_RED, GL_UNSIGNED_BYTE, frame_data[2]);
	return true;
}

void Yuv420pRender::RenderFrame()
{
	glClearColor(0.f, 0.f, 0.f, 0.f);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_tex_y);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_tex_u);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_tex_v);

	glUseProgram(m_shader_pro);
	glBindVertexArray(m_vertex_array);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	
	while (!glfwWindowShouldClose(m_wnd))
	{
		glfwSwapBuffers(m_wnd);
		glfwPollEvents();
	}
	
	return ;
}

bool Yuv420pRender::InitEnviroment()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	m_wnd = glfwCreateWindow(m_wnd_width, m_wnd_height, "Yuv420pRender", nullptr, nullptr);
	if (nullptr == m_wnd)
	{
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(m_wnd);
	glfwSetFramebufferSizeCallback(m_wnd, window_size_callback);
	//glfwSetInputMode(m_wnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return false;
	}

	return true;
}

bool Yuv420pRender::InitShader()
{
	uint32_t vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader_id, 1, &vertex_shader, nullptr);
	glCompileShader(vertex_shader_id);
	int shader_rec;
	glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &shader_rec);
	if (0 != shader_rec)
	{
		return false;
	}

	uint32_t frage_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frage_shader_id, 1, &frage_shader, nullptr);
	glCompileShader(frage_shader_id);
	glGetShaderiv(frage_shader_id, GL_COMPILE_STATUS, &shader_rec);
	if (0 != shader_rec)
	{
		return false;
	}

	m_shader_pro = glCreateProgram();
	glAttachShader(m_shader_pro, vertex_shader_id);
	glAttachShader(m_shader_pro, frage_shader_id);
	glLinkProgram(m_shader_pro);
	
	glGetProgramiv(m_shader_pro, GL_LINK_STATUS, &shader_rec);
	if (0 != shader_rec) 
	{
		return false;
	}
	glDeleteShader(vertex_shader_id);
	glDeleteShader(frage_shader_id);

	float vertex_coord_data[] = {
		-1.f, -1.f, 0.f,   0.f, 0.f,
		-1.f,  1.f, 0.f,   0.f, 1.f,
		 1.f,  1.f, 0.f,   1.f, 1.f,
		 1.f, -1.f, 0.f,   1.f, 0.f,
	};

	float vertx_index_data[] = {
		0, 1, 2,
		2, 3, 1
	};

	uint32_t gl_vertex_buffer, gl_index_buffer;
	glGenBuffers(1, &gl_vertex_buffer);
	glGenBuffers(1, &gl_index_buffer);

	glGenVertexArrays(1, &m_vertex_array);
	glBindVertexArray(m_vertex_array);

	glBindBuffer(GL_ARRAY_BUFFER, gl_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_coord_data), vertex_coord_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertx_index_data), vertx_index_data, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glUseProgram(m_shader_pro);

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_tex_width / 2, m_wnd_height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glGenerateMipmap(GL_TEXTURE_2D);

	glUniform1i(glGetUniformLocation(m_shader_pro, "tex_y"), 0);
	glUniform1i(glGetUniformLocation(m_shader_pro, "tex_u"), 1);
	glUniform1i(glGetUniformLocation(m_shader_pro, "tex_v"), 2);
	return true;
}

void Yuv420pRender::window_size_callback(GLFWwindow* win, int width, int height)
{
	glViewport(0, 0, width, height);
	return;
}
