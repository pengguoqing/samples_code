/*
* create data: 2023-02-28
* this class is the implement for factory pure virtual class
  it is use to create ffreader factory
* contact info: forwardapeng@gmail.com
*/

#pragma once
#include"factorybase.h"

namespace mediaio
{
  
class IOFactory:public IFactory {

public:
  IOFactory()  = default;
  ~IOFactory() = default;

public:
  IClipReader* CreateReader() override;
  
};

}
