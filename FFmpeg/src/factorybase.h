/*
* create data: 2023-02-28
* this is pure virtual factory class to create mediaIO 
* contact info: forwardapeng@gmail.com
*/

#pragma once

#include "readerbase.h"

namespace mediaio
{
    class IFactory
    {
      public:
        virtual IClipReader* CreateReader() = 0;
    };
}

__declspec(dllexport) mediaio::IFactory* CreateMediaIOFactory();