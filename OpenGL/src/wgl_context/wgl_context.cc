#include "wgl_context.h"
typedef PROC (WINAPI*   PFNWGLGETPROCADDRESSPROC) (LPCSTR lpszProc);
typedef BOOL (WINAPI*   PFNWGLMAKECURRENTPROC) (HDC hdc, HGLRC hglrc);
typedef HGLRC (WINAPI*  PFNWGLCREATECONTEXTPROC) (HDC hdc);
typedef BOOL  (WINAPI*  PFNWGLDELETECONTEXTPROC) (HGLRC hglrc);
typedef void (*WGLProc)(void);

struct WglExtension
{
	WglExtension() = default;
	WglExtension(const WglExtension& that ) = delete;
	WglExtension& operator=(const WglExtension& that) = delete;
	bool Init(){
		m_gl_lib = LoadLibraryA("opengl32.dll");
		if (nullptr == m_gl_lib){
			return false;
		}
	
		m_wgl_get_proc_address	= reinterpret_cast<PFNWGLGETPROCADDRESSPROC> (GetProcAddress(m_gl_lib, "wglGetProcAddress") );
		m_wgl_make_ctx 			= reinterpret_cast<PFNWGLMAKECURRENTPROC>(GetProcAddress(m_gl_lib, "wglMakeCurrent") );
		m_wgl_create_ctx		= reinterpret_cast<PFNWGLCREATECONTEXTPROC>(GetProcAddress(m_gl_lib, "wglCreateContext") );
		m_wgl_delete_ctx		= reinterpret_cast<PFNWGLDELETECONTEXTPROC>(GetProcAddress(m_gl_lib, "wglDeleteContext"));
		m_wgl_choosepix_arb		= reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(GetProcAddress(m_gl_lib, "wglChoosePixelFormatARB"));
		m_wgl_createctx_ext_arb	= reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(GetProcAddress(m_gl_lib, "wglCreateContextAttribsARB"));
		m_wgl_swapinterval		= reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(GetProcAddress(m_gl_lib, "wglSwapIntervalEXT"));
		

    	return true;
	}

	~WglExtension()
	{
		::FreeLibrary(m_gl_lib);
	}

	HMODULE         			m_gl_lib{nullptr};
	PFNWGLGETPROCADDRESSPROC 	m_wgl_get_proc_address{nullptr};
	PFNWGLMAKECURRENTPROC		m_wgl_make_ctx{nullptr};
	PFNWGLCREATECONTEXTPROC		m_wgl_create_ctx{nullptr};
	PFNWGLDELETECONTEXTPROC		m_wgl_delete_ctx{nullptr};
	PFNWGLCHOOSEPIXELFORMATARBPROC	  	m_wgl_choosepix_arb{nullptr};
	PFNWGLCREATECONTEXTATTRIBSARBPROC 	m_wgl_createctx_ext_arb{nullptr};
	PFNWGLSWAPINTERVALEXTPROC			m_wgl_swapinterval{nullptr};
};

static WglExtension g_wgl_extension;

WGLProc WglGetProcAddress(const char* procname){
	
	if (nullptr == g_wgl_extension.m_wgl_get_proc_address){
	     return nullptr;
	}
	
	WGLProc corrfuncpointer = reinterpret_cast<WGLProc>( g_wgl_extension.m_wgl_get_proc_address(procname) );
	if(corrfuncpointer){
		return corrfuncpointer;
	}else{
		return reinterpret_cast<WGLProc> (::GetProcAddress(g_wgl_extension.m_gl_lib, procname) );
	}

	return nullptr;
}

static HGLRC CreateContext(HDC _hdc)
{
		PIXELFORMATDESCRIPTOR pfd;
		std::memset(&pfd, 0, sizeof(pfd) );
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cAlphaBits = 8;
		pfd.cDepthBits = 24;
		pfd.cStencilBits = 8;
		pfd.iLayerType = PFD_MAIN_PLANE;

		int pixelFormat = ::ChoosePixelFormat(_hdc, &pfd);
		::DescribePixelFormat(_hdc, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
		::SetPixelFormat(_hdc, pixelFormat, &pfd);
		HGLRC context = g_wgl_extension.m_wgl_create_ctx(_hdc);
		g_wgl_extension.m_wgl_make_ctx(_hdc, context);
		return context;
}

bool GLContext::InitGLRC()
{
	//load opengl32.dll 
	if(!g_wgl_extension.Init()){
		return false;
	}	
    // 先创建一个空窗口, 因为有了窗口设备及其GL句柄绑定后才能使用高阶 wglChoosePixelFormatARB 接口
    std::string dummyclass{"dummyclass"};
    RegisterWndClass(dummyclass);
    HWND dummywnd = CreateWindowA(0, dummyclass.c_str(), WS_POPUP, 0, 0, 2, 2, nullptr, nullptr, GetModuleHandle(nullptr), 0);
    HDC dummyhdc = GetDC(dummywnd);
    HGLRC dummy_glctx = CreateContext(dummyhdc);
   
    // 加载 GLAD_WGL
    // if (!gladLoadWGL(dummyhdc, WglGetProcAddress)) {
    //     return false;
    // }
	if (!gladLoadGL(WglGetProcAddress)){
		 return false;
	}
	
	g_wgl_extension.m_wgl_make_ctx(nullptr, nullptr);
	g_wgl_extension.m_wgl_delete_ctx(dummy_glctx);
	::DestroyWindow(dummywnd);
	
	if (!InitPlatDevice()){
		return false;
	}
	
	
    return true;
}

bool GLContext::RegisterWndClass(const std::string &classname)
{
		
    WNDCLASSA wc;
	memset(&wc, 0, sizeof(wc));
	wc.style = CS_OWNDC;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpfnWndProc = DefWindowProc;
	wc.lpszClassName = classname.c_str();

	if (!RegisterClassA(&wc)) {
		return false;
	}

	return true;
}


bool GLContext::InitPlatDevice()
{
	std::string plat_wnd_class{"plat_wnd_class"};
    RegisterWndClass(plat_wnd_class);
	m_plat_wnd 	=	::CreateWindowA(0, plat_wnd_class.c_str(), WS_POPUP, 0, 0, 2, 2, nullptr, nullptr, GetModuleHandle(nullptr), 0);
    m_plat_hdc 	=  	::GetDC(m_plat_wnd);

	m_pixel_format = ChoosePixelFormat(m_plat_hdc);
	::DescribePixelFormat(m_plat_hdc, m_pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &m_pfd);
	bool ret= ::SetPixelFormat(m_plat_hdc, m_pixel_format, &m_pfd);
	if(!ret){
		unsigned long error = GetLastError();
		return false;
	}

	const int ctx_attribs[] = {

#ifdef _DEBUG
		WGL_CONTEXT_FLAGS_ARB,
		WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
		WGL_CONTEXT_PROFILE_MASK_ARB,
		WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		WGL_CONTEXT_MAJOR_VERSION_ARB,
		4,
		WGL_CONTEXT_MINOR_VERSION_ARB,
		4,
		0,
		0};

	m_plat_glctx = g_wgl_extension.m_wgl_createctx_ext_arb(m_plat_hdc, 0, ctx_attribs);
	if(nullptr == m_plat_glctx){
		return false;
	}

	if(!g_wgl_extension.m_wgl_make_ctx(m_plat_hdc, m_plat_glctx) ){
		return false;
	}

   	return true;
}

int GLContext::ChoosePixelFormat(const HDC hdc) const 
{

	int32_t attrs[] =
	{
		WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
		WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,

		WGL_ALPHA_BITS_ARB,     8,
		WGL_COLOR_BITS_ARB,     32,
		WGL_DEPTH_BITS_ARB,     24,
		WGL_STENCIL_BITS_ARB,   8,

		WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
		WGL_SAMPLES_ARB,        0,
		WGL_SAMPLE_BUFFERS_ARB, GL_FALSE,

		0
	};

	int result{0};
	uint32_t numFormats{0};
	int pixel_format{0};
	do
	{
		if (nullptr == g_wgl_extension.m_wgl_choosepix_arb) {
			break;
		}
		
		result = g_wgl_extension.m_wgl_choosepix_arb(hdc, attrs, nullptr, 1, &pixel_format, &numFormats);
		
		if (0 == result ||  0 == numFormats) {
			attrs[3] >>= 1;
			attrs[1] = attrs[3] == 0 ? 0 : 1;
		}

	} while (0 == numFormats);
		
	
    return pixel_format;
}
