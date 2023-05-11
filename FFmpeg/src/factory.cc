#include "factory.h"
#include "ffreader.h"


namespace mediaio {

IClipReader* mediaio::IOFactory::CreateReader() {
    return new FFReader();
}

void mediaio::IOFactory::Release(void* rw){
     delete rw;
}

}


mediaio::IFactory* CreateMediaIOFactory() {
    return new mediaio::IOFactory;
}

void  ReleaeFactory(void* factory) {
    delete factory; 
}