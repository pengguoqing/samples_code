////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "applicationclass.h"
#include <memory>
#include <utility>
#include <array>

////////////////////////////////////////////////////////////////////////////////
// Class Name: SystemClass
////////////////////////////////////////////////////////////////////////////////
class SystemClass
{
public:
    SystemClass();
    SystemClass(const SystemClass&);
    ~SystemClass();

    bool Initialize();
    void Shutdown();
    void Frame();

private:
    bool InitializeWindow(int&, int&);
    void ShutdownWindow();
    void ReadInput();

private:
    std::unique_ptr<ApplicationClass>     m_Application;
    std::unique_ptr<InputClass>           m_Input;
    GLFWwindow*                           m_videoDisplay{ nullptr };
};

#endif

