#include <iostream>
#include <string>
#include <array>
#include <fstream>
#include <memory>
#include <vector>
#include <chrono>
#include "fbo.h"
#include "pbo.h"
#include "glfw/glfw3.h"
#include "stb_image.h"
#include "shader_parse.hpp"
#include "libyuv.h"

using namespace std;

#define USE_PBO  1
static constexpr int wndwidth{1920};
static constexpr int wndheight{1080};
static constexpr  uint32_t imgwidth{1280};
static constexpr  uint32_t imgheight{960};

void      callback_winsize(GLFWwindow* winhandle, int width, int height);
void      processInput(GLFWwindow *window);
uint32_t  PrepareYUYVtex();

int main(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* winhandle = glfwCreateWindow(wndwidth, wndheight, "fbo_pbo", nullptr, nullptr);
    glfwMakeContextCurrent(winhandle);
    glfwSetFramebufferSizeCallback(winhandle, callback_winsize);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    array<float, 20> vertices{
        -1.f,  1.f, 0.f,  0.f, 0.f,
        1.f,   1.f,  0.f, 1.f, 0.f,
        1.f,  -1.f, 0.f,  1.f, 1.f,
        -1.f, -1.f, 0.f,  0.f, 1.f
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
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), nullptr);
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

uint32_t  PrepareYUYVtex()
{
    //read binary yuyv data
    ifstream yuyvfile {"../../../res/fbo.yuyv", ios::binary|ios::in};
    std::vector<uint8_t> buffer{ std::istreambuf_iterator<char>(yuyvfile), {}};
    uint8_t* imgptr = &buffer.front();
    uint32_t linesizeimg = 1280*CXPbo::GetPixfmtBpp(GL_RGBA)/8;

    //create tex to upload yuyv
    uint32_t uploadtex;
    glGenTextures(1, &uploadtex);
    glBindTexture(GL_TEXTURE_2D, uploadtex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, imgwidth/2, imgheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    auto begin = chrono::high_resolution_clock::now();
#ifdef USE_PBO
    //use pbo to upload img
    CXPbo  pbo;
    pbo.Init(imgwidth, imgheight, PBOTYPE::kDynamic, GL_RGBA);
    uint8_t* dstptr{nullptr};
    uint32_t linesizeout{0};
    pbo.Map(&dstptr, &linesizeout);
    if (linesizeout == linesizeimg){
        memcpy_s(dstptr, linesizeout*imgheight, imgptr, linesizeimg*imgheight);
    }
    else{
        for(size_t lines=0; lines<imgheight; lines++)
        {
            memcpy_s(dstptr, linesizeout, imgptr, linesizeimg);
            dstptr += linesizeout;
            imgptr += linesizeimg;
        }
    }   
    pbo.UnMap(uploadtex);
#else
    glBindTexture(GL_TEXTURE_2D, uploadtex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imgwidth/2, imgheight, GL_RGBA, GL_UNSIGNED_BYTE, imgptr);
    glBindTexture(0);
#endif
    auto end = chrono::high_resolution_clock::now();
    cout<<"upload time is "<< chrono::duration<double, milli>(end-begin).count() <<endl;

    return uploadtex;
}