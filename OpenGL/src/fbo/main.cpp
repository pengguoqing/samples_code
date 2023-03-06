#include <iostream>
#include <string>
#include <array>
#include "fbo.h"
#include "glfw/glfw3.h"
#include "stb_image.h"
#include "shader_parse.hpp"
#include "libyuv.h"

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

    GLuint imgtex = CreateImgTexture("../../res/fbo.jpg");
   
    array<float, 20> vertices{
        -1.f,  1.f, 0.f, 0.f, 0.f,
        1.f,   1.f,  0.f, 1.f, 0.f,
        1.f,  -1.f, 0.f, 1.f, 1.f,
        -1.f, -1.f, 0.f, 0.f, 1.f
    };

    array<GLuint, 6> indices{
        0, 1, 2,
        2, 3, 0
    };

    GLuint vbo{0}, ebo{0}, vao{0};
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices.front(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices.front(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    ShaderParse uploadshader;
    uploadshader.InitShader("fbo.vs", "fbo.fs");
    uploadshader.use();

    while (!glfwWindowShouldClose(winhandle))
    {
        processInput(winhandle);
		
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
        uploadshader.use();
        glBindVertexArray(vao);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, texfmt, imgwidth, imgheight, 0, texfmt, GL_UNSIGNED_BYTE, imgdata);
        glGenerateMipmap(GL_TEXTURE_2D);    
    }
    
    stbi_image_free(imgdata);
    return imgtex;
}
