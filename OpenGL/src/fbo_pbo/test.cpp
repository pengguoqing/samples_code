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
#include "glm/glm.hpp"

using namespace std;

#define USE_PBO  0
static constexpr int wndwidth{1920};
static constexpr int wndheight{1080};
static constexpr  uint32_t imgwidth{1280};
static constexpr  uint32_t imgheight{960};

static const glm::vec4 color_vec0(1.16438353f, 0.00000000f,  1.79274106f, -0.972945154f);
static const glm::vec4 color_vec1(1.16438353f, -0.213248610f, -0.532909334f, 0.301482677f);
static const glm::vec4 color_vec2(1.16438353f, 2.11240172f,  0.f, -1.13340223f);
static const glm::vec3 color_range_min(0.0627451017f, 0.0627451017f, 0.0627451017f);
static const glm::vec3 color_range_max(0.921568632f, 0.941176474f, 0.941176474f);


void      callback_winsize(GLFWwindow* winhandle, int width, int height);
void      processInput(GLFWwindow *window);
bool      UploadYUYV(const CXPbo& pbo, const vector<uint8_t>&imgdata, uint32_t uploadtex);

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

    array<float, 20> vertex{
        -1.f,  1.f, 0.f,  0.f, 0.f,
        1.f,   1.f, 0.f, 1.f, 0.f,
        1.f,  -1.f, 0.f,  1.f, 1.f,
        -1.f, -1.f, 0.f,  0.f, 1.f
    };

    array<GLuint, 6> indx{
        0, 1, 2,
        2, 3, 0
    };

    //upload vao
    GLuint vbo{0}, ebo{0}, uploadvao{0};
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &uploadvao);
    glBindVertexArray(uploadvao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), &vertex.front(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indx), &indx.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    //render vao
    GLuint rendervbo{0}, renderebo{0}, rendervao{0};
    glGenBuffers(1, &rendervbo);
    glGenVertexArrays(1, &rendervao);
    glBindVertexArray(rendervao);
    glBindBuffer(GL_ARRAY_BUFFER, rendervbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), &vertex.front(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendervao);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex), &vertex.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    //Init upload and render shader
    ShaderParse uploadshader;
    uploadshader.InitShader("../fbo_pbo.vs", "../fbo_pbo.fs");
    uploadshader.use();
    uploadshader.setInt("image", 0);
    uploadshader.setFloat("width_offset", 0.5f / imgwidth);
    
    //ShaderParse rendershader;
    //rendershader.InitShader("../render.vs", "../render.fs");
    //rendershader.use();
    //rendershader.setInt("rendertex", 0);
    
    //read binary yuyv data
    ifstream yuyvfile {"../../../res/fbo.yuyv", ios::binary|ios::in};
    std::vector<uint8_t> imgdata{ std::istreambuf_iterator<char>(yuyvfile), {}};
    
    CXPbo yuy2pbo;
    yuy2pbo.Init(imgwidth/2, imgheight, PBOTYPE::kDynamic, GL_RGBA);
    
   /* CXFbo yuy2fbo;
    yuy2fbo.InitFbo(imgwidth, imgheight);*/

    GLuint uploadtex;
    glGenTextures(1, &uploadtex);
    glBindTexture(GL_TEXTURE_2D, uploadtex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, imgwidth/2, imgheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    while (!glfwWindowShouldClose(winhandle))
    {
        processInput(winhandle);
		
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
        //yuy2fbo.BindFbo();
        uploadshader.use();
        glBindVertexArray(uploadvao);
        //glBindTexture(GL_TEXTURE_2D, uploadtex);
        //UploadYUYV(yuy2pbo, imgdata, uploadtex);
        
        //glBindTexture(GL_TEXTURE_2D, uploadtex);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //yuy2fbo.UnBindFbo();

        /*rendershader.use();
        glBindVertexArray(rendervao);
        rendershader.use();
        yuy2fbo.BindColorTexture();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        yuy2fbo.UnBindColorTexture();*/

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
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
}

bool   UploadYUYV(const CXPbo& pbo, const vector<uint8_t>&imgdata, uint32_t uploadtex)
{
    uint32_t linesizeimg = imgwidth * CXPbo::GetPixfmtBpp(GL_RGBA)/8/2;
    uint8_t* imgdataptr  = const_cast<uint8_t*>(&imgdata.front());

    auto begin = chrono::high_resolution_clock::now();
#if USE_PBO
    //use pbo to upload img
    uint8_t* dstptr{nullptr};
    uint32_t linesizeout{0};
    pbo.Map(&dstptr, &linesizeout);
    if (linesizeout == linesizeimg){
        memcpy_s(dstptr, linesizeout*imgheight, imgdataptr, linesizeimg*imgheight);
    }
    else{
        for(size_t lines=0; lines<imgheight; lines++)
        {
            memcpy_s(dstptr, linesizeout, imgdataptr, linesizeimg);
            dstptr += linesizeout;
            imgdataptr += linesizeimg;
        }
    }   
    pbo.UnMap(uploadtex);
    
#else
    glBindTexture(GL_TEXTURE_2D, uploadtex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imgwidth/2, imgheight, GL_RGBA, GL_UNSIGNED_BYTE, imgdataptr);
    glBindTexture(GL_TEXTURE_2D, 0);
#endif
    auto end = chrono::high_resolution_clock::now();
    cout<<"upload time is "<< chrono::duration<double, milli>(end-begin).count() <<endl;
    return true;
}