#ifndef _CPLUSPLUS_PBO_H_
#define _CPLUSPLUS_PBO_H_

#include "glad/glad.h"
#include <memory>

enum class PBOTYPE:uint8_t
{   
    kDynamic,
    kStage,
};

class CXPbo
{
public:
    inline CXPbo();
    ~CXPbo();

    //just move, can not copy
    CXPbo(const CXPbo& another) = delete;
    CXPbo& operator=(const CXPbo& another) = delete;
    inline CXPbo(CXPbo&& another);
    inline CXPbo& operator=(CXPbo&& another);

public:
    bool Init(uint32_t width, uint32_t height, PBOTYPE type, GLenum pixfmt);

private:
    void     Swap(CXPbo& another);
    uint32_t GetPixfmtBpp(GLenum pixfmt);
private:
    uint32_t m_pbo;
    uint32_t m_width;
    uint32_t m_hegit;
    uint32_t m_pixfmt;
};

#endif