#include "fbo.h"

CXFbo::CXFbo()
:m_width(0),
 m_height(0),
 m_fbo(0),
 m_colortex(0),
 m_rbo(0)
{}

CXFbo::~CXFbo()
{
   DeleteFboBuf();
}

CXFbo::CXFbo(CXFbo &&another)
:CXFbo()
{
    this->Swap(another);
}

CXFbo& CXFbo::operator=(CXFbo &&another)
{
    this->Swap(another);
    return *this;
}

bool CXFbo::InitFbo(int width, int height, int msaa)
{
    if (width<=0 || height<=0)
    {
        return false;
    }
    
    int maxmsaa{0};
    glGetIntegerv(GL_MAX_SAMPLES, &maxmsaa);
    if(msaa > maxmsaa) { msaa = maxmsaa;}
    if (msaa < 0)      { msaa = 0;}
    if (msaa%2 != 0)   { msaa -= 1;}

    m_width   = width;
    m_height  = height;
    m_msaa    = msaa;

    DeleteFboBuf();                     
    m_colordata.reset(new uint8_t[m_width*m_height*4]{});
    m_depthdata.reset(new float[m_width*m_height]{});
    
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &m_colortex);
    glBindTexture(GL_TEXTURE_2D, m_colortex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colortex, 0);

    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

    GLenum fbostatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    return GL_FRAMEBUFFER_COMPLETE == fbostatus;
}


void CXFbo::BindFbo()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void CXFbo::UnBindFbo()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CXFbo::BindColorTexture()
{
    glBindTexture(GL_TEXTURE_2D, m_colortex);
}

void CXFbo::UnBindColorTexture()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void CXFbo::Swap(CXFbo &another)
{
    std::swap(this->m_fbo, another.m_fbo);
    std::swap(this->m_rbo, another.m_rbo);
    std::swap(this->m_colortex, another.m_colortex);
}

void CXFbo::DeleteFboBuf()
{
    if(m_colortex)
    {       
        glDeleteTextures(1, &m_colortex);
        m_colortex = 0;
    }   

    if(m_rbo)
    {
        glDeleteRenderbuffers(1, &m_rbo);
        m_rbo = 0;
    }
    
    if (m_fbo)
    {
        glDeleteFramebuffers(1, &m_fbo);
        m_fbo = 0;
    }
    
}


