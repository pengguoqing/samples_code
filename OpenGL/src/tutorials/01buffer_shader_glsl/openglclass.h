////////////////////////////////////////////////////////////////////////////////
// Filename: openglclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _OPENGLCLASS_H_
#define _OPENGLCLASS_H_


//////////////
// INCLUDES //
//////////////
//Glfw glad includes
#include "glad/gl.h"
#define WIN32_LEAN_AND_MEAN
#include "glad/wgl.h"
#include "glfw/glfw3.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#define GLFW_EXPOSE_NATIVE_WIN32
//#define GLFW_EXPOSE_NATIVE_WGL
//#include "glfw/glfw3native.h"

#include <string.h>
#include <math.h>
#include <array>
#include <utility>

////////////////////////////////////////////////////////////////////////////////
// Class Name: OpenGLClass
////////////////////////////////////////////////////////////////////////////////
class OpenGLClass
{
public:
    OpenGLClass();
    OpenGLClass(const OpenGLClass&);
    ~OpenGLClass();

    bool Initialize(GLFWwindow*, int, int, float, float, bool);
    void Shutdown();

    void BeginScene(float, float, float, float);
    void EndScene();

    glm::mat4 GetWorldMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
   
    void MatrixRotationX(float*, float);
    void MatrixRotationY(float*, float);
    void MatrixRotationZ(float*, float);
    void MatrixTranslation(float*, float, float, float);
    void MatrixScale(float*, float, float, float);
    void MatrixTranspose(float*, float*);
    void MatrixMultiply(float*, float*, float*);

    void TurnZBufferOn();
	void TurnZBufferOff();

	void EnableAlphaBlending();
	void DisableAlphaBlending();

	void SetBackBufferRenderTarget();
	void ResetViewport();

	void EnableClipping();
	void DisableClipping();

private:
    GLFWwindow* m_display{nullptr};
    HWND        m_native_wnd{nullptr};
    HDC         m_device_ctx{nullptr};
    HGLRC       m_gl_ctx{nullptr};

    glm::mat4   m_worldMatrix{1.f};
    glm::mat4   m_projectionMatrix{1.f};
    float m_screenWidth{0.f}, m_screenHeight{0.f};
};

#endif

