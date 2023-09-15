////////////////////////////////////////////////////////////////////////////////
// Filename: openglclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "openglclass.h"


OpenGLClass::OpenGLClass()
{
}


OpenGLClass::OpenGLClass(const OpenGLClass& other)
{
}


OpenGLClass::~OpenGLClass()
{
}


bool OpenGLClass::Initialize(GLFWwindow* display, int screenWidth, int screenHeight, float screenNear, float screenDepth, bool vsync)
{
    // Store the screen size.
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;


    m_display = display;
    glfwMakeContextCurrent(m_display);

    gladLoadGL(/*glfwGetProcAddress*/);

    // Get a handle to the root window.
    //m_native_wnd = glfwGetWin32Window(m_display);
    //m_gl_ctx     = glfwGetWGLContext(m_display);
    
    //PIXELFORMATDESCRIPTOR pfd;
    //memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
    //pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
    //pfd.nVersion   = 1;
    //pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    //pfd.iPixelType = PFD_TYPE_RGBA;
    //pfd.cColorBits = 32;
    //pfd.cDepthBits = 24;
    //pfd.cStencilBits = 8;
    //pfd.iLayerType = PFD_MAIN_PLANE;

    //m_device_ctx   = ::GetDC(m_native_wnd);
    //gladLoadWGLLoader((GLADloadproc)(glfwGetProcAddress), m_device_ctx);

    //int pixfmt     = ::ChoosePixelFormat(m_device_ctx, &pfd);
    ////DescribePixelFormat(m_device_ctx, pixfmt, sizeof(pfd), &pfd);
    //bool rec       = ::SetPixelFormat(m_device_ctx, pixfmt, &pfd);
    //if (!rec){
    //   int rec = GetLastError();
    //   return false;
    //}
    //
    //m_gl_ctx       = wglCreateContext(m_device_ctx);
    //wglMakeCurrent(m_device_ctx, m_gl_ctx);
    //
    //std::array<std::pair<int, int>, 4> gl_version{ { {4, 4}, {4, 3}, {4, 2}} };
    //for (const auto& item:gl_version){
    //            int MajorVersion        = item.first;
    //            int MinorVersion        = item.second;
    //        
    //            int attribs[] =
    //                {
    //                    WGL_CONTEXT_MAJOR_VERSION_ARB, MajorVersion,
    //                    WGL_CONTEXT_MINOR_VERSION_ARB, MinorVersion,
    //                   	WGL_CONTEXT_PROFILE_MASK_ARB,
		  //              WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
    //                    0, 0 
    //                };

    //            // Create new rendering context
    //            // In order to create new OpenGL rendering context we have to call function wglCreateContextAttribsARB(),
    //            // which is an OpenGL function and requires OpenGL to be active when it is called.
    //            // The only way is to create an old context, activate it, and while it is active create a new one.
    //            // Very inconsistent, but we have to live with it!
    //            HGLRC new_gl_ctx = wglCreateContextAttribsARB(m_device_ctx, 0, attribs);
    //            if (new_gl_ctx){
    //                wglMakeCurrent(nullptr, nullptr);
    //                wglDeleteContext(m_gl_ctx);
    //                m_gl_ctx = new_gl_ctx;
    //                wglMakeCurrent(m_device_ctx, m_gl_ctx);
    //                break;
    //            }
    //            
    //}
    //

    // Set the depth buffer to be entirely cleared to 1.0 values.
    glClearDepth(1.0f);

    // Enable depth testing.
    glEnable(GL_DEPTH_TEST);

    // Set the polygon winding to clockwise front facing for the left handed system.
    glFrontFace(GL_CW);

    // Enable back face culling.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

  
    // Turn on or off the vertical sync depending on the input bool value.
	if(vsync){
        glfwSwapInterval(1);
	}else{
        glfwSwapInterval(0);
	}

    m_projectionMatrix = glm::perspective(glm::radians(45.0f), m_screenWidth / m_screenHeight, 0.1f, 100.0f);
    
    return true;
}


void OpenGLClass::Shutdown()
{
    //wglMakeCurrent(nullptr, nullptr);
    //wglDeleteContext(m_gl_ctx);
    //ReleaseDC(m_native_wnd, m_device_ctx);
    return;
}


void OpenGLClass::BeginScene(float red, float green, float blue, float alpha)
{
    // Set the color to clear the screen to.
    glClearColor(red, green, blue, alpha);

    // Clear the screen and depth buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return;
}


void OpenGLClass::EndScene()
{
    // Present the back buffer to the screen since rendering is complete.
    glfwSwapBuffers(m_display);
    glfwPollEvents();
    return;
}

glm::mat4 OpenGLClass::GetWorldMatrix() const
{
    return m_worldMatrix;
}

glm::mat4 OpenGLClass::GetProjectionMatrix() const
{
    return m_projectionMatrix;
}

void OpenGLClass::MatrixRotationX(float* matrix, float angle)
{
	matrix[0]  = 1.0f;
	matrix[1]  = 0.0f;
	matrix[2]  = 0.0f;
	matrix[3]  = 0.0f;

	matrix[4]  = 0.0f;
	matrix[5]  = cosf(angle);
	matrix[6]  = sinf(angle);
	matrix[7]  = 0.0f;

	matrix[8]  = 0.0f;
	matrix[9]  = -sinf(angle);
	matrix[10] = cosf(angle);
	matrix[11] = 0.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 1.0f;

	return;
}


void OpenGLClass::MatrixRotationY(float* matrix, float angle)
{
	matrix[0]  = cosf(angle);
	matrix[1]  = 0.0f;
	matrix[2]  = -sinf(angle);
	matrix[3]  = 0.0f;

	matrix[4]  = 0.0f;
	matrix[5]  = 1.0f;
	matrix[6]  = 0.0f;
	matrix[7]  = 0.0f;

	matrix[8]  = sinf(angle);
	matrix[9]  = 0.0f;
	matrix[10] = cosf(angle);
	matrix[11] = 0.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 1.0f;

	return;
}


void OpenGLClass::MatrixRotationZ(float* matrix, float angle)
{
	matrix[0]  = cosf(angle);
	matrix[1]  = sinf(angle);
	matrix[2]  = 0.0f;
	matrix[3]  = 0.0f;

	matrix[4]  = -sinf(angle);
	matrix[5]  = cosf(angle);
	matrix[6]  = 0.0f;
	matrix[7]  = 0.0f;

	matrix[8]  = 0.0f;
	matrix[9]  = 0.0f;
	matrix[10] = 1.0f;
	matrix[11] = 0.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 1.0f;

	return;
}


void OpenGLClass::MatrixTranslation(float* matrix, float x, float y, float z)
{
	matrix[0]  = 1.0f;
	matrix[1]  = 0.0f;
	matrix[2]  = 0.0f;
	matrix[3]  = 0.0f;

	matrix[4]  = 0.0f;
	matrix[5]  = 1.0f;
	matrix[6]  = 0.0f;
	matrix[7]  = 0.0f;

	matrix[8]  = 0.0f;
	matrix[9]  = 0.0f;
	matrix[10] = 1.0f;
	matrix[11] = 0.0f;

	matrix[12] = x;
	matrix[13] = y;
	matrix[14] = z;
	matrix[15] = 1.0f;

	return;
}


void OpenGLClass::MatrixScale(float* matrix, float x, float y, float z)
{
	matrix[0]  = x;
	matrix[1]  = 0.0f;
	matrix[2]  = 0.0f;
	matrix[3]  = 0.0f;

	matrix[4]  = 0.0f;
	matrix[5]  = y;
	matrix[6]  = 0.0f;
	matrix[7]  = 0.0f;

	matrix[8]  = 0.0f;
	matrix[9]  = 0.0f;
	matrix[10] = z;
	matrix[11] = 0.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 1.0f;

	return;
}


void OpenGLClass::MatrixTranspose(float* result, float* matrix)
{
 	result[0]  = matrix[0];
	result[1]  = matrix[4];
	result[2]  = matrix[8];
	result[3]  = matrix[12];

	result[4]  = matrix[1];
	result[5]  = matrix[5];
	result[6]  = matrix[9];
	result[7]  = matrix[13];

	result[8]  = matrix[2];
	result[9]  = matrix[6];
	result[10] = matrix[10];
	result[11] = matrix[14];

	result[12] = matrix[3];
	result[13] = matrix[7];
	result[14] = matrix[11];
	result[15] = matrix[15];

    return;
}


void OpenGLClass::MatrixMultiply(float* result, float* matrix1, float* matrix2)
{
	result[0]  = (matrix1[0] * matrix2[0]) + (matrix1[1] * matrix2[4]) + (matrix1[2] * matrix2[8]) + (matrix1[3] * matrix2[12]);
	result[1]  = (matrix1[0] * matrix2[1]) + (matrix1[1] * matrix2[5]) + (matrix1[2] * matrix2[9]) + (matrix1[3] * matrix2[13]);
	result[2]  = (matrix1[0] * matrix2[2]) + (matrix1[1] * matrix2[6]) + (matrix1[2] * matrix2[10]) + (matrix1[3] * matrix2[14]);
	result[3]  = (matrix1[0] * matrix2[3]) + (matrix1[1] * matrix2[7]) + (matrix1[2] * matrix2[11]) + (matrix1[3] * matrix2[15]);

	result[4]  = (matrix1[4] * matrix2[0]) + (matrix1[5] * matrix2[4]) + (matrix1[6] * matrix2[8]) + (matrix1[7] * matrix2[12]);
	result[5]  = (matrix1[4] * matrix2[1]) + (matrix1[5] * matrix2[5]) + (matrix1[6] * matrix2[9]) + (matrix1[7] * matrix2[13]);
	result[6]  = (matrix1[4] * matrix2[2]) + (matrix1[5] * matrix2[6]) + (matrix1[6] * matrix2[10]) + (matrix1[7] * matrix2[14]);
	result[7]  = (matrix1[4] * matrix2[3]) + (matrix1[5] * matrix2[7]) + (matrix1[6] * matrix2[11]) + (matrix1[7] * matrix2[15]);

	result[8]  = (matrix1[8] * matrix2[0]) + (matrix1[9] * matrix2[4]) + (matrix1[10] * matrix2[8]) + (matrix1[11] * matrix2[12]);
	result[9]  = (matrix1[8] * matrix2[1]) + (matrix1[9] * matrix2[5]) + (matrix1[10] * matrix2[9]) + (matrix1[11] * matrix2[13]);
	result[10] = (matrix1[8] * matrix2[2]) + (matrix1[9] * matrix2[6]) + (matrix1[10] * matrix2[10]) + (matrix1[11] * matrix2[14]);
	result[11] = (matrix1[8] * matrix2[3]) + (matrix1[9] * matrix2[7]) + (matrix1[10] * matrix2[11]) + (matrix1[11] * matrix2[15]);

	result[12] = (matrix1[12] * matrix2[0]) + (matrix1[13] * matrix2[4]) + (matrix1[14] * matrix2[8]) + (matrix1[15] * matrix2[12]);
	result[13] = (matrix1[12] * matrix2[1]) + (matrix1[13] * matrix2[5]) + (matrix1[14] * matrix2[9]) + (matrix1[15] * matrix2[13]);
	result[14] = (matrix1[12] * matrix2[2]) + (matrix1[13] * matrix2[6]) + (matrix1[14] * matrix2[10]) + (matrix1[15] * matrix2[14]);
	result[15] = (matrix1[12] * matrix2[3]) + (matrix1[13] * matrix2[7]) + (matrix1[14] * matrix2[11]) + (matrix1[15] * matrix2[15]);

	return;
}


void OpenGLClass::TurnZBufferOn()
{
    // Enable depth testing.
    glEnable(GL_DEPTH_TEST);

    return;
}


void OpenGLClass::TurnZBufferOff()
{
    // Disable depth testing.
    glDisable(GL_DEPTH_TEST);

    return;
}


void OpenGLClass::EnableAlphaBlending()
{
    // Enable alpha blending.
    glEnable(GL_BLEND);

    // Set the blending equation.
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    return;
}


void OpenGLClass::DisableAlphaBlending()
{
    // Disable alpha blending.
    glDisable(GL_BLEND);

    return;
}


void OpenGLClass::SetBackBufferRenderTarget()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return;
}


void OpenGLClass::ResetViewport()
{
    glViewport(0, 0, m_screenWidth, m_screenHeight);
    return;
}


void OpenGLClass::EnableClipping()
{
	// Enable clip plane 0.
    glEnable(GL_CLIP_DISTANCE0);

    return;
}


void OpenGLClass::DisableClipping()
{
	// Disable clip plane 0.
    glDisable(GL_CLIP_DISTANCE0);

    return;
}
