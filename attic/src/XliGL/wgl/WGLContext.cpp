//
// Copyright (C) 2010-2014 Outracks Technologies
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include <XliGL.h>
#include <XliPlatform/PlatformSpecific/Win32.h>
#include <XliPlatform/Window.h>
#include <Xli/Console.h>
#include <Xli/Shared.h>
#include "../../../3rdparty/glew/include/GL/wglew.h"

namespace Xli
{
    namespace PlatformSpecific
    {
        static bool Inited = false;

        class WGLContext : public GLContext
        {
            HDC hDC;
            HWND hWnd;
            HGLRC hGLRC;
            PIXELFORMATDESCRIPTOR pfd;
            int pf;

            int TryEnableMultisample(const GLContextAttributes& attribs)
            {
                if (!Inited)
                {
                    static const PIXELFORMATDESCRIPTOR tmpPfd =
                    {
                        sizeof(PIXELFORMATDESCRIPTOR),
                        1,
                        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
                        PFD_TYPE_RGBA,
                        32,
                        0, 0, 0, 0, 0, 0,
                        0, // No alpha buffer
                        0,
                        0, // No accumulation buffer
                        0, 0, 0, 0,
                        24, // 24-bit z buffer
                        0, // No stencil buffer
                        0, // No auxilary buffer
                        PFD_MAIN_PLANE,
                        0, 0, 0, 0
                    };

                    HWND tmpWnd = CreateWindowEx(0, L"STATIC", L"", 0, 0, 0, 16, 16, hWnd, 0, GetModuleHandle(0), 0);
                    HDC tmpDC = GetDC(tmpWnd);
                    int fmt = ChoosePixelFormat(tmpDC, &tmpPfd);
                    SetPixelFormat(tmpDC, fmt, &pfd);
                    HGLRC tmpGL = wglCreateContext(tmpDC);
                    wglMakeCurrent(tmpDC, tmpGL);

                    glewInit();

                    wglMakeCurrent(tmpDC, 0);
                    wglDeleteContext(tmpGL);
                    ReleaseDC(tmpWnd, tmpDC);
                    DestroyWindow(tmpWnd);

                    Inited = true;
                }

                if (attribs.Samples <= 1)
                {
                    return -1;
                }

                int iattribs[] =
                {
                    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
                    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
                    WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
                    WGL_RED_BITS_ARB, attribs.ColorBits.R,
                    WGL_GREEN_BITS_ARB, attribs.ColorBits.G,
                    WGL_BLUE_BITS_ARB, attribs.ColorBits.B,
                    WGL_ALPHA_BITS_ARB, attribs.ColorBits.A,
                    WGL_ACCUM_RED_BITS_ARB, attribs.AccumBits.R,
                    WGL_ACCUM_GREEN_BITS_ARB, attribs.AccumBits.G,
                    WGL_ACCUM_BLUE_BITS_ARB, attribs.AccumBits.B,
                    WGL_ACCUM_ALPHA_BITS_ARB, attribs.AccumBits.A,
                    WGL_DEPTH_BITS_ARB, attribs.DepthBits,
                    WGL_STENCIL_BITS_ARB, attribs.StencilBits,
                    WGL_DOUBLE_BUFFER_ARB, attribs.Buffers > 1 ? GL_TRUE : GL_FALSE,
                    WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
                    WGL_SAMPLES_ARB, attribs.Samples,
                    0, 0
                };

                // TODO: attribs.Stereo is not handled.

                float fattribs[] = { 0, 0 };

                UINT numConfigs;
                int configs[64];

                while (true)
                {
                    if (!wglChoosePixelFormatARB(hDC, iattribs, fattribs, 64, configs, &numConfigs) || numConfigs == 0)
                    {
                        if (iattribs[19] > 2)
                        {
                            iattribs[19] /= 2;
                            continue;
                        }

                        Error->WriteLine("WGL WARNING: Unable to find any multisampled OpenGL formats");
                        return -1;
                    }

                    break;
                }

                if (!SetPixelFormat(hDC, configs[0], &pfd))
                {
                    Error->WriteLine("WGL ERROR: Unable to set multisampled OpenGL format: " + Win32::GetLastErrorString());
                    return -1;
                }

                return configs[0];
            }

        public:
            WGLContext(Window* wnd, const GLContextAttributes& attribs)
            {
                hWnd = Win32::GetWindowHandle(wnd);
                hDC = GetDC(hWnd);

                ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
                pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
                pfd.nVersion = 1;
                pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;

                if (attribs.Stereo)
                    pfd.dwFlags |= PFD_STEREO;

                pfd.iPixelType = PFD_TYPE_RGBA;
                pfd.cRedBits = attribs.ColorBits.R;
                pfd.cGreenBits = attribs.ColorBits.G;
                pfd.cBlueBits = attribs.ColorBits.B;
                pfd.cAlphaBits = attribs.ColorBits.A;
                pfd.cAccumRedBits = attribs.AccumBits.R;
                pfd.cAccumGreenBits = attribs.AccumBits.G;
                pfd.cAccumBlueBits = attribs.AccumBits.B;
                pfd.cAccumAlphaBits = attribs.AccumBits.A;
                pfd.cDepthBits = attribs.DepthBits;
                pfd.cStencilBits = attribs.StencilBits;
                pfd.iLayerType = PFD_MAIN_PLANE;

                pf = TryEnableMultisample(attribs);

                if (pf == -1 && !SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd), &pfd))
                    XLI_THROW("Failed to create OpenGL context: " + Win32::GetLastErrorString());

                hGLRC = wglCreateContext(hDC);
                
                if (!hGLRC)
                    XLI_THROW("Failed to create OpenGL context: " + Win32::GetLastErrorString());

                MakeCurrent(wnd);

                glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                SwapBuffers();
            }

            WGLContext(WGLContext* srcCtx)
            {
                hWnd = srcCtx->hWnd;
                hDC = srcCtx->hDC;
                pf = srcCtx->pf;
                memcpy(&pfd, &srcCtx->pfd, sizeof(PIXELFORMATDESCRIPTOR));

                hGLRC = wglCreateContext(hDC);

                if (!hGLRC)
                    XLI_THROW("Unable to create shared OpenGL context: " + Win32::GetLastErrorString());

                if (!wglMakeCurrent(0, 0))
                    XLI_THROW("Unable to make OpenGL context no longer current: " + Win32::GetLastErrorString());

                if (!wglShareLists(srcCtx->hGLRC, hGLRC))
                    XLI_THROW("Unable to share OpenGL contexts: " + Win32::GetLastErrorString());

                if (!wglMakeCurrent(srcCtx->hDC, srcCtx->hGLRC))
                    XLI_THROW("Unable to make OpenGL context current: " + Win32::GetLastErrorString());
            }

            virtual ~WGLContext()
            {
                wglDeleteContext(hGLRC);
            }

            virtual GLContext* CreateSharedContext()
            {
                return new WGLContext(this);
            }

            virtual void MakeCurrent(Window* wnd)
            {
                if (wnd && Win32::GetWindowHandle(wnd) != hWnd)
                {
                    hWnd = Win32::GetWindowHandle(wnd);
                    hDC = GetDC(hWnd);

                    if (pf != -1)
                        SetPixelFormat(hDC, pf, &pfd);
                    else
                        SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd), &pfd);
                }

                if (!wglMakeCurrent(hDC, wnd ? hGLRC : 0))
                    XLI_THROW("Unable to make OpenGL context current: " + Win32::GetLastErrorString());

                if (wnd && pf != -1)
                    glEnable(GL_MULTISAMPLE_ARB);
            }

            virtual bool IsCurrent()
            {
                return wglGetCurrentContext() == hGLRC;
            }

            virtual void SwapBuffers()
            {
                ::SwapBuffers(hDC);
            }

            virtual void SetSwapInterval(int interval)
            {
                wglSwapIntervalEXT && wglSwapIntervalEXT(interval);
            }

            virtual int GetSwapInterval()
            {
                return wglGetSwapIntervalEXT ? wglGetSwapIntervalEXT() : -1;
            }

            virtual Vector2i GetDrawableSize()
            {
                RECT rect;
                GetClientRect(hWnd, &rect);
                return Vector2i(rect.right, rect.bottom);
            }

            virtual void GetAttributes(GLContextAttributes& result)
            {
                int iattribs[] =
                {
                    WGL_RED_BITS_ARB, WGL_GREEN_BITS_ARB, WGL_BLUE_BITS_ARB, WGL_ALPHA_BITS_ARB,
                    WGL_DEPTH_BITS_ARB,
                    WGL_STENCIL_BITS_ARB,
                    WGL_SAMPLES_ARB,
                    WGL_ACCUM_RED_BITS_ARB, WGL_ACCUM_GREEN_BITS_ARB, WGL_ACCUM_BLUE_BITS_ARB, WGL_ACCUM_ALPHA_BITS_ARB,
                };

                ZeroMemory(&result, sizeof(GLContextAttributes));
                wglGetPixelFormatAttribivARB(hDC, pf, 0, 11, iattribs, (int*)&result);

                // TODO
                result.Buffers = 2;
                result.Stereo = false;
            }
        };
    }

    GLContext* GLContext::Create(Window* window, const GLContextAttributes& attribs)
    {     
        return new PlatformSpecific::WGLContext(window, attribs);
    }
}
