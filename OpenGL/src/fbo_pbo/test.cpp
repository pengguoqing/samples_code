#include <iostream>
#include <string>
#include <array>
#include <fstream>
#include <memory>
#include <vector>
#include <chrono>
#include <thread>
#include "fbo.h"
#include "pbo.h"
#include "glfw/glfw3.h"
#include "stb_image.h"
#include "shader_parse.hpp"
#include "glm/glm.hpp"

#define USE_PBO  false

static constexpr int wndwidth{960};
static constexpr int wndheight{540};
static constexpr  uint32_t imgwidth{1280};
static constexpr  uint32_t imgheight{960};

static const glm::vec4 color_vec0(1.16438353f, 0.00000000f,  1.79274106f, -0.972945154f);
static const glm::vec4 color_vec1(1.16438353f, -0.213248610f, -0.532909334f, 0.301482677f);
static const glm::vec4 color_vec2(1.16438353f, 2.11240172f,  0.f, -1.13340223f);
static const glm::vec3 color_range_min(0.0627451017f, 0.0627451017f, 0.0627451017f);
static const glm::vec3 color_range_max(0.921568632f, 0.941176474f, 0.941176474f);


void      callback_winsize(GLFWwindow* winhandle, int width, int height);
void      processInput(GLFWwindow *window);
bool      UploadYUYV(const CXPbo& pbo, const std::vector<uint8_t>&imgdata, uint32_t uploadtex);

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

    std::array<float, 20> vertex{
        -1.f,  1.f, 0.f,  0.f, 1.f,
        1.f,   1.f, 0.f,  1.f, 1.f,
        1.f,  -1.f, 0.f,  1.f, 0.f,
        -1.f, -1.f, 0.f,  0.f, 0.f
    };

    std::array<GLuint, 6> indx{
        0, 1, 2,
        0, 3, 2
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
	GLuint rendervbo{ 0 }, renderebo{ 0 }, rendervao{ 0 };
	glGenBuffers(1, &rendervbo);
    glGenBuffers(1, &renderebo);
	glGenVertexArrays(1, &rendervao);
	glBindVertexArray(rendervao);
	glBindBuffer(GL_ARRAY_BUFFER, rendervbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), &vertex.front(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indx), &indx.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

    //Init upload and render shader
    ShaderParse uploadshader;
    uploadshader.InitShader("../fbo_pbo.vs", "../fbo_pbo.fs");
    uploadshader.use();
    uploadshader.setInt("image", 0);
    uploadshader.setFloat("width_offset", 0.5f / imgwidth);
    uploadshader.setFloat("width_d2", imgwidth / 2);
    uploadshader.setFloat("height",   imgheight);
    uploadshader.setVec3("color_range_min", color_range_min);
    uploadshader.setVec3("color_range_max", color_range_max);
    uploadshader.setVec4("color_vec0",      color_vec0);
    uploadshader.setVec4("color_vec1",      color_vec1);
    uploadshader.setVec4("color_vec2",      color_vec2);

    ShaderParse rendershader;
    rendershader.InitShader("../render.vs", "../render.fs");
    rendershader.use();
    rendershader.setInt("rendertex", 0);
    
    //read binary yuyv data
    std::ifstream yuyvfile {"../../../res/fbo.yuyv", std::ios::binary| std::ios::in};
    std::vector<uint8_t> imgdata{ std::istreambuf_iterator<char>(yuyvfile), {}};

	GLuint uploadtex;
	glGenTextures(1, &uploadtex);
	glBindTexture(GL_TEXTURE_2D, uploadtex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgwidth/2, imgheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    CXPbo yuy2pbo;
    yuy2pbo.Init(imgwidth/2, imgheight, PBOTYPE::kDynamic, GL_RGBA);
    
	CXFbo yuy2fbo;
	yuy2fbo.InitFbo(imgwidth, imgheight);
    yuy2fbo.UnBindFbo();
    glfwSwapInterval(0);
    while (!glfwWindowShouldClose(winhandle))
    {
        processInput(winhandle);
        glViewport(0, 0, imgwidth, imgheight);
        
        yuy2fbo.BindFbo();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
        uploadshader.use();
        UploadYUYV(yuy2pbo, imgdata, uploadtex);

		glBindVertexArray(uploadvao);
		glBindTexture(GL_TEXTURE_2D, uploadtex);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        yuy2fbo.UnBindFbo();
        glBindVertexArray(0);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, wndwidth, wndheight);
        rendershader.use();
        glBindVertexArray(rendervao);
        yuy2fbo.BindColorTexture();
        glGenerateMipmap(GL_TEXTURE_2D);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        yuy2fbo.UnBindColorTexture();
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

bool   UploadYUYV(const CXPbo& pbo, const std::vector<uint8_t>&imgdata, uint32_t uploadtex)
{
    uint32_t linesizeimg = imgwidth * CXPbo::GetPixfmtBpp(GL_RGBA)/8/2;
    uint8_t* imgdataptr  = const_cast<uint8_t*>(&imgdata.front());

    glBindTexture(GL_TEXTURE_2D, uploadtex);
#if USE_PBO
    //use pbo to upload image
    uint8_t* dstptr{nullptr};
    uint32_t linesizeout{0};

    pbo.Map(&dstptr, &linesizeout);
    std::cout << "ptr : " << static_cast<void*>(dstptr) << std::endl;

    if (linesizeout == linesizeimg){
        memcpy_s(dstptr, static_cast<rsize_t>(linesizeout)*imgheight, imgdataptr, static_cast<rsize_t>(linesizeimg)*imgheight);
    }
    else{
        for(size_t lines=0; lines<imgheight; lines++)
        {
            memcpy_s(dstptr, linesizeout, imgdataptr, linesizeimg);
            dstptr += linesizeout;
            imgdataptr += linesizeimg;
        }
    }  
    pbo.UnMap();
    const auto start = std::chrono::high_resolution_clock::now();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imgwidth/2, imgheight, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    pbo.UnBind();

#else
    const auto start = std::chrono::high_resolution_clock::now();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, imgwidth/2, imgheight, GL_RGBA, GL_UNSIGNED_BYTE, imgdataptr);
#endif

    glBindTexture(GL_TEXTURE_2D, 0);

	const auto end = std::chrono::high_resolution_clock::now();
	std::cout << "upload time : " << std::chrono::duration<double, std::milli>(end - start).count() << std::endl;
   
    return true;
}