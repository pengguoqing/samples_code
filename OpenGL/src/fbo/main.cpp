#include <iostream>
#include <string>
#include "fbo.h"
#include "glfw/glfw3.h"
#include "stb_image.h"

using namespace std;

constexpr int wndwidth{1920};
constexpr int wndheight{1080};

void    callback_winsize(GLFWwindow* winhandle, int width, int height);
void    processInput(GLFWwindow *window);
GLuint  CreateImgTexture(string filename);

int main(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* winhandle = glfwCreateWindow(wndwidth, wndheight, "fbo", nullptr, nullptr);
    glfwMakeContextCurrent(winhandle);
    glfwSetFramebufferSizeCallback(winhandle, callback_winsize);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);





    while (!glfwWindowShouldClose(winhandle))
    {
        processInput(winhandle);
        glfwSwapBuffers(winhandle);
        glfwPollEvents();
    }
    
    return 0;
}


void callback_winsize(GLFWwindow* winhandle, int width, int height){

    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

GLuint  CreateImgTexture(string filename)
{
    GLuint imgtex{0};
    int imgwidth{0}, imgheight{0}, channel{0};
    uint8_t* imgdata = stbi_load(filename.c_str(), &imgwidth, &imgheight, &channel, 0);
    if (imgdata)
    {   
        GLenum texfmt{0};
        if (1==channel)
        {
            texfmt = GL_RED;
        }
        else if (3 == channel)
        {
            texfmt = GL_RGB;
        }
        else if (4 == channel)
        {
            texfmt = GL_RGBA;
        }
        
        glGenTextures(1, &imgtex);
        glBindTexture(GL_TEXTURE_2D, imgtex);
        glTexImage2D(GL_TEXTURE_2D, 0, texfmt, imgwidth, imgheight, 0, texfmt, GL_UNSIGNED_BYTE, imgdata);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    
    stbi_image_free(imgdata);
    return imgtex;
}
