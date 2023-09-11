////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"


ApplicationClass::ApplicationClass()
{
    m_OpenGL = 0;
    m_ColorShader = 0;
    m_Model = 0;
    m_Camera = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(GLFWwindow* display, int screenWidth, int screenHeight)
{
    
    // Create and initialize the OpenGL object.
    m_OpenGL = std::make_shared<OpenGLClass>();

    bool result = m_OpenGL->Initialize(display, screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH, VSYNC_ENABLED);
    if(!result) {
        return false;
    }

    // Create and initialize the color shader object.
    m_ColorShader = std::make_unique<ColorShaderClass>();

    result = m_ColorShader->Initialize(m_OpenGL);
    if(!result)
    {
        return false;
    }

    // Create and initialize the camera object.
    m_Camera = std::make_unique<CameraClass>();

    m_Camera->SetPosition(0.0f, 0.0f, 5.0f);
    m_Camera->Render();

    // Create and initialize the model object.
    m_Model = std::make_unique<ModelClass>();

    result = m_Model->Initialize(m_OpenGL);
    if(!result){
        return false;
    }


    return true;
}


void ApplicationClass::Shutdown()
{
    // Release the model object.
    if(m_Model){
        m_Model->Shutdown();
        m_Model.reset();
    }

    // Release the camera object.
    if(m_Camera){
        m_Camera.reset();
    }

    // Release the color shader object.
    if(m_ColorShader){
        m_ColorShader->Shutdown();
        m_ColorShader.reset();
    }

    // Release the OpenGL object.
    if(m_OpenGL){
        m_OpenGL->Shutdown();
        m_OpenGL.reset();
    }

    return;
}


bool ApplicationClass::Frame(const InputClass& Input)
{
    bool result;


    // Check if the escape key has been pressed, if so quit.
    if(Input.IsEscapePressed() == true)
    {
        return false;
    }

    // Render the graphics scene.
    result = Render();
    if(!result)
    {
        return false;
    }

    return true;
}


bool ApplicationClass::Render()
{
    // Clear the buffers to begin the scene.
    m_OpenGL->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // Get the world, view, and projection matrices from the opengl and camera objects.
    glm::mat4 worldmat      = m_OpenGL->GetWorldMatrix();
    glm::mat4 viewmat       = m_Camera->GetViewMatrix();
    glm::mat4 projectmat    = m_OpenGL->GetProjectionMatrix();

    // Set the color shader as the current shader program and set the matrices that it will use for rendering.
    bool result = m_ColorShader->SetShaderParameters(worldmat, viewmat, projectmat);
    if(!result)
    {
        return false;
    }
    
    // Render the model.
    m_Model->Render();

    // Present the rendered scene to the screen.
    m_OpenGL->EndScene();

    return true;
}
