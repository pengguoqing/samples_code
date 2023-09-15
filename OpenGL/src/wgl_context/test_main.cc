#include "wgl_context.h"
#include "glfw/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include "glfw/glfw3native.h"

int main(int argc, char**argv)
{
	GLContext plat_ctx;
	plat_ctx.InitGLRC();

    glfwInit();
    GLFWwindow* main_window = glfwCreateWindow(480, 270, "mainwindow", nullptr, nullptr);
    HWND main_wnd = glfwGetWin32Window(main_window);
    
    GLFWwindow* sub_window = glfwCreateWindow(480, 270, "subwindow", nullptr, nullptr);
	HWND sub_wnd  = glfwGetWin32Window(sub_window);

    plat_ctx.RegisterWnd(main_wnd);
    plat_ctx.RegisterWnd(sub_wnd);

    while (!glfwWindowShouldClose(main_window) ) {

        if (nullptr!=sub_window && glfwWindowShouldClose(sub_window) ){
            plat_ctx.UnRegisterWnd(sub_wnd);
			glfwDestroyWindow(sub_window);
			sub_window = nullptr;
        }
        plat_ctx.AttachWindow(main_wnd);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
        plat_ctx.Present(main_wnd);

        plat_ctx.AttachWindow(sub_wnd);
        glClearColor(0.f, 0.f, 0.5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        plat_ctx.Present(sub_wnd);

        glfwPollEvents();
    }

    glfwDestroyWindow(main_window);
    glfwDestroyWindow(sub_window);
    return 0;
}