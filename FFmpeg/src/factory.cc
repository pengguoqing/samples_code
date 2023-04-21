#include "factory.h"
#include "ffreader.h"

namespace mediaio {

IClipReader *mediaio::IOFactory::CreateReader() {
    return nullptr;
}

}