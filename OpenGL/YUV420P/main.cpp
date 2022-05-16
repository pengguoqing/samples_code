#include "Yuv420pRender.h"
#include "ffmpeg_reader.h"
#include <thread>
#include <chrono>
#include <string>

using namespace std;

static FFmpegReader   clip_reader;
static Yuv420pRender  video_render;
static int            wnd_width = 1280;
static int            wnd_height = 720;

void window_size_callback(GLFWwindow* win, int width, int height);
bool InitClipReader(const string& filename);
bool InitGLRender();

int main(int agrc, char* argv[])
{
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* wnd = glfwCreateWindow(wnd_width, wnd_height, "Yuv420pRender", nullptr, nullptr);
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
	vector<uint8_t*> framedata(4);
	while (!glfwWindowShouldClose(wnd))
	{
		clip_reader.ReadVideoFrame(framedata, frameindex++);
		video_render.UpLoadFrame(framedata);
		video_render.RenderFrame();
		
		glfwSwapBuffers(wnd);
		glfwPollEvents();
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

  FrameDataParam param;
  param.m_width = wnd_width;
  param.m_height = wnd_height;
  param.m_type = MetaDataType::kVideo;
  param.m_pixfmt = PixFmt::kYUV420P;

  return clip_reader.SetFrameParam(param);

}

bool InitGLRender()
{
	return video_render.InitRender(wnd_width, wnd_height);
}