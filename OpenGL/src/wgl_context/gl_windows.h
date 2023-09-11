#ifndef _CPLUS_GLWINDOW_H_
#define _CPLUS_GLWINDOW_H_
#include <windows.h>

using NativeWinHandle = HWND;
using NativeWinRenderCtx = HGLRC;
NativeWinHandle CreateNativeWindow(int width, int height);

#endif