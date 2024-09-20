#ifndef _CPLUSPLUS_PBO_H_
#define _CPLUSPLUS_PBO_H_

#include "glad/glad.h"
#include <memory>

enum class PBOTYPE:uint8_t
{  
    kUnknown,
    kDynamic,
    kStage,
};

class CXPbo
{
public:
     CXPbo();
    ~CXPbo();

    //just move, can not copy
    CXPbo(const CXPbo& another) = delete;
    CXPbo& operator=(const CXPbo& another) = delete;
    inline CXPbo(CXPbo&& another) noexcept;
    inline CXPbo& operator=(CXPbo&& another) noexcept;

public:
    bool Init(uint32_t width, uint32_t height, PBOTYPE type, GLenum pixfmt);
    void Map(uint8_t** ptr, uint32_t* linesize) const;
    void UnMap() const;
    void Bind() const;
    void UnBind() const;

static uint32_t GetPixfmtBpp(GLenum pixfmt);

private:
    void     Swap(CXPbo& another);
    
private:
    uint32_t m_pbo;
    uint32_t m_width;
    uint32_t m_hegit;
    uint32_t m_pixfmt;
    uint32_t m_rwtype;
    int64_t  m_pbosize;
};

#endif