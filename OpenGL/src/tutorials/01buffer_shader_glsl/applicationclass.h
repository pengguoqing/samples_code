////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_


/////////////
// GLOBALS //
/////////////
constexpr bool FULL_SCREEN{false};
constexpr bool VSYNC_ENABLED{true};
constexpr float SCREEN_NEAR{0.3f};
constexpr float SCREEN_DEPTH{1000.f};


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "inputclass.h"
#include "openglclass.h"
#include "colorshaderclass.h"
#include "modelclass.h"
#include "cameraclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class Name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////
class ApplicationClass
{
public:
    ApplicationClass();
    ApplicationClass(const ApplicationClass&);
    ~ApplicationClass();

    bool Initialize(GLFWwindow*, int, int);
    void Shutdown();
    bool Frame(const InputClass& Input);

private:
    bool Render();

private:
    shared_ptr<OpenGLClass>      m_OpenGL;
    unique_ptr<ColorShaderClass> m_ColorShader;
    unique_ptr<ModelClass>       m_Model;
    unique_ptr<CameraClass>      m_Camera;
};

#endif

