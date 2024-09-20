#include "yuv422_render.h"
#include "ffmpeg_reader.h"
#include <thread>
#include <chrono>
#include <string>

using namespace std;

static FFmpegReader   clip_reader;
static YUV422Render   video_render;
static int            wnd_width = 1280;
static int            wnd_height = 720;

void window_size_callback(GLFWwindow* win, int width, int height);
bool InitClipReader(const string& filename);
bool InitGLRender();

int main(int agrc, char* argv[])
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* wnd = glfwCreateWindow(wnd_width, wnd_height, "YUV422Render", nullptr, nullptr);
	if (nullptr == wnd)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(wnd);
	glfwSetFramebufferSizeCallback(wnd, window_size_callback);
	glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return -1;
	}

	if (!InitClipReader(argv[1]) || !InitGLRender())
	{
		return -1;
	}

	int frameindex = 0;

	FrameInfo frame;
	clip_reader.GetVideoFrame(frame, frameindex++);

	while (!glfwWindowShouldClose(wnd))
	{
		glClearColor(0.f, 0.f, 0.4f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);

		video_render.UpLoadFrame(frame.data[0]);
		video_render.RenderFrame();

		glfwSwapBuffers(wnd);
		glfwPollEvents();
		clip_reader.GetVideoFrame(frame, frameindex++);

		this_thread::sleep_for(chrono::milliseconds(40));

	}

	return 0;
}


void window_size_callback(GLFWwindow* win, int width, int height)
{
	glViewport(0, 0, width, height);
	return;
}

bool InitClipReader(const string& filename)
{
	if (!clip_reader.InitAVFmt(filename))
	{
		return false;
	}

	media_params media_info = clip_reader.GetMediaInfo();

	ReaderParam param;
	param.m_width = wnd_width;
	param.m_height = wnd_height;
	param.m_type = MetaDataType::kVideo;
	param.m_pixfmt = PixFmt::kYUV422;

	return clip_reader.SetFrameParam(param);

}

bool InitGLRender()
{
	return video_render.InitRender(wnd_width, wnd_height);
}