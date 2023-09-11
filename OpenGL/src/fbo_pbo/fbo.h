#ifndef _CPLUSPLUS_FBO_H_
#define _CPLUSPLUS_FBO_H_

#include "glad/gl.h"
#include <memory>

class CXFbo
{
public:
    CXFbo();
    ~CXFbo();

    //just move, can not copy
    CXFbo(const CXFbo& aother)   = delete;
    CXFbo& operator = (const CXFbo& another) = delete; 
    inline CXFbo(CXFbo&& another) noexcept;
    inline CXFbo& operator = (CXFbo&& another) noexcept;

public:
    bool InitFbo(int width, int height, int msaa=0);
    void BindFbo();
    void UnBindFbo();
    void BindColorTexture();
    void UnBindColorTexture();
    
private:
    void Swap(CXFbo& another);
    void DeleteFboBuf();
private:

    // buffer w and h
    int     m_width;
    int     m_height;

    int     m_msaa;
    std::unique_ptr<uint8_t[]>  m_colordata;
    std::unique_ptr<float[]>    m_depthdata;
    GLuint  m_fbo;
    GLuint  m_colortex;
    GLuint  m_rbo;
};






#endif