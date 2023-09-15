#ifndef _CPLUS_WGL_CONTEXT_H_
#define _CPLUS_WGL_CONTEXT_H_

//#define WIN32_LEAN_AND_MEAN 

#include "glad/glad.h"
#include "glad/glad_wgl.h"
#include <unordered_map>
#include <string>
#include <functional>


class GLContext
{
public:    
    GLContext()  = default;
    ~GLContext() = default;
    GLContext(const GLContext& that) = default;
    GLContext(GLContext&& that) = default;
    GLContext& operator = (const GLContext& that) = default;
    GLContext& operator = (GLContext&& that) = default;

public:
    bool InitGLRC();
    bool RegisterWnd(HWND hwnd);
    bool UnRegisterWnd(HWND hwnd);
    bool AttachWindow(HWND hwnd);
    bool Present(HWND hwnd);

private:
    bool RegisterWndClass(const std::string& classname);
private:
    bool InitPlatDevice();
    int  ChoosePixelFormat(const HDC hdc) const;

private:
    HGLRC           m_plat_glctx{nullptr};
    HDC             m_plat_hdc{nullptr};
    HWND            m_plat_wnd{nullptr};
    int             m_pixel_format{0};
    PIXELFORMATDESCRIPTOR m_pfd;
    std::unordered_map<HWND, HDC> m_windevice_table;
        
};




#endif