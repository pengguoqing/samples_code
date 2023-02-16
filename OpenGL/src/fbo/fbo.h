#ifndef _CPLUSPLUS_FBO_H_
#define _CPLUSPLUS_FBO_H_

#include "glad/glad.h"
#include <memory>

class CXFbo
{
public:
    CXFbo();
    ~CXFbo();

    //just move, can not copy
    CXFbo(CXFbo&& another);
    CXFbo& operator = (CXFbo&& another);

public:
    bool InitFbo(int width, int height, int msaa=0);
    void Bind();
    void UnBind();

private:
    void DeleteFboBuf();
private:

    // buffer w and h
    int     m_width;
    int     m_height;

    int     m_msaa;
    std::unique_ptr<uint8_t[]>  m_colordata;
    std::unique_ptr<float[]>    m_depthdata;
    GLuint  m_fbo;
    GLuint  m_tex;
    GLuint  m_rbo;
};






#endif