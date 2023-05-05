#include "factory.h"
#include "ffreader.h"


namespace mediaio {

IClipReader *mediaio::IOFactory::CreateReader() {
    return new FFReader();
}

}
    
mediaio::IFactory* CreateMediaIOFactory() {

    return new mediaio::IOFactory;
}