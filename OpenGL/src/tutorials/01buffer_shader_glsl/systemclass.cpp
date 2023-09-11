////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"


SystemClass::SystemClass()
{
}


SystemClass::SystemClass(const SystemClass& other)
{
}


SystemClass::~SystemClass()
{
}


bool SystemClass::Initialize()
{
    // Create and initialize the input object.
    m_Input = std::make_unique<InputClass>();
    m_Input->Initialize();

    // Initialize the screen size.
    int screenWidth{0}, screenHeight{0};
    
    bool result = InitializeWindow(screenWidth, screenHeight);
    if(!result) {
        return false;
    }

    // Create and initialize the application object.
    m_Application = std::make_unique<ApplicationClass>();

    result = m_Application->Initialize(m_videoDisplay, screenWidth, screenHeight);
    if(!result)
    {
        return false;
    }

    return true;
}


void SystemClass::Shutdown()
{
    // Release the application object.
    if(m_Application){
        m_Application->Shutdown();
        m_Application.reset();
    }

    // Release the X11 window.
    ShutdownWindow();

    // Release the input object.
    if(m_Input){
        m_Input.reset();
    }

    return;
}


void SystemClass::Frame()
{
    // Loop until the application is finished running.
   
    while(!glfwWindowShouldClose(m_videoDisplay))
    {
        ReadInput();

        m_Application->Frame(*m_Input);
    }

    return;
}


bool SystemClass::InitializeWindow(int& screenWidth, int& screenHeight)
{
    if (!glfwInit()){
        return false;
    }

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create native on the primary monitor.
    GLFWmonitor* monitor =  glfwGetPrimaryMonitor();
    if (FULL_SCREEN){
        glfwGetMonitorPhysicalSize(monitor, &screenWidth, &screenHeight);

    }else{
        screenWidth  = 480;
        screenHeight = 270;
    }
    
    m_videoDisplay = glfwCreateWindow(screenWidth, screenHeight, "01buffer_shader_glsl", nullptr, nullptr);
    if(nullptr == m_videoDisplay){
        return false;
    }
 
    return true;
}


void SystemClass::ShutdownWindow()
{
    // Shutdown and close the window.
    glfwDestroyWindow(m_videoDisplay);
    return;
}


void SystemClass::ReadInput()
{
    if(GLFW_PRESS == glfwGetKey(m_videoDisplay, GLFW_KEY_ESCAPE) ){
        m_Input->KeyDown(65307);     
    }

    if(GLFW_RELEASE == glfwGetKey(m_videoDisplay, GLFW_KEY_ESCAPE) ){
        m_Input->KeyUp(65307);     
    }
   
    return;
}
