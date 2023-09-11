#include "gl_windows.h"

NativeWinHandle CreateNativeWindow(int width, int height) {
    
    return CreateWindowExA(0, nullptr, nullptr,
				    WS_POPUP, 0, 0, width, height, nullptr, nullptr,
				    GetModuleHandle(nullptr), nullptr);
}