#include "pbo.h"



CXPbo::CXPbo()
:m_pbo(0),
 m_width(0),
 m_hegit(0),
 m_pixfmt(0),
 m_rwtype(0)
{
}

CXPbo::~CXPbo()
{
    if(m_pbo){
        glDeleteBuffers(1, &m_pbo);
    }

}

CXPbo::CXPbo(CXPbo &&another)
:CXPbo()
{
    this->Swap(another);
}

inline CXPbo &CXPbo::operator=(CXPbo &&another)
{
   this->Swap(another);
   return *this;
}

bool CXPbo::Init(uint32_t width, uint32_t height, PBOTYPE type, GLenum pixfmt)
{
   m_width  = width;
   m_width  = height;
   m_pixfmt = pixfmt;
   
   switch (type)
   {
   case PBOTYPE::kDynamic:
        m_rwtype = GL_PIXEL_UNPACK_BUFFER;
    break;
   case PBOTYPE::kStage:
        m_rwtype = GL_PIXEL_PACK_BUFFER;
    break;
   default:
        return false;
    break;
   }
   

   glGenBuffers(1, &m_pbo);
   if (!m_pbo)
   {
     return false;
   }
    
   glBindBuffer(m_rwtype, m_pbo);

   GLsizeiptr size = m_width * GetPixfmtBpp(pixfmt) / 8;
   size = (size + 3) & 0xFFFFFFFC;
   size *= m_hegit;
   glBufferData(m_rwtype, size, nullptr, GL_DYNAMIC_DRAW); 

   glBindBuffer(m_rwtype, 0);

   return false;
}

void CXPbo::Map(uint8_t **ptr, uint32_t *linesize)
{
    glBindBuffer(m_rwtype, m_pbo);

    GLuint access = GL_PIXEL_UNPACK_BUFFER == m_rwtype ? GL_WRITE_ONLY : GL_READ_ONLY;
    *ptr = static_cast<uint8_t*>( glMapBuffer(m_rwtype, access) );
    
    *linesize = m_width * GetPixfmtBpp(m_pixfmt) / 8;
    *linesize = (*linesize + 3) & 0xFFFFFFFC;    
}

void CXPbo::UnMap(uint32_t uploadtex)
{
    glBindBuffer(m_rwtype, m_pbo);
    glUnmapBuffer(m_rwtype);
    if (uploadtex)
    {
        glBindTexture(GL_TEXTURE_2D, uploadtex);
        glTexImage2D(GL_TEXTURE_2D, 0, m_pixfmt, m_width, m_hegit, 0, m_pixfmt, GL_UNSIGNED_BYTE, nullptr);
    }

    glBindBuffer(m_rwtype, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void CXPbo::Swap(CXPbo &another)
{
    std::swap(this->m_pbo, another.m_pbo);
    std::swap(this->m_width, another.m_width);
    std::swap(this->m_hegit, another.m_hegit);
    std::swap(this->m_pixfmt,another.m_pixfmt);
}

uint32_t CXPbo::GetPixfmtBpp(GLenum pixfmt)
{
    switch (pixfmt)
    {
    case GL_RGBA:
        return 32;
        break;
    
    default:
        break;
    }
    return 0;
}
