/*
* create data: 2023-02-28
* this is pure virtual factory class to create mediaIO 
* contact info: forwardapeng@gmail.com
*/

#pragma once

#include "readerbase.h"
namespace mediaIO
{
    class IXRFactory
    {
    public:
        virtual ~IXRFactory() = 0;
        virtual IXMediaReader* CreateReader() = 0;
    };
}
