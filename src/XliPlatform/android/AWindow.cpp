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

#include "../../../3rdparty/android_native_app_glue/android_native_app_glue.h"

#include <android/window.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <cstdlib>
#include <errno.h>
#include <unistd.h>

#include <XliPlatform/PlatformSpecific/Android.h>
#include <XliPlatform/InputEventHandler.h>
#include <XliPlatform/Application.h>
#include <XliPlatform/Display.h>
#include <XliPlatform/Window.h>
#include <XliGL/GLContext.h>
#include <Xli/Console.h>
#include "AJniHelper.h"
#include "AWindow.h"
#include "AShim.h"

#include <Xli/Console.h>

static int GlobalFlags = 0;

namespace Xli
{
    namespace PlatformSpecific
    {
        AWindow::AWindow()
        {
            PrintLine("~~~~~~~~~~~~~~~~~ Window Construct");
        }

        AWindow::~AWindow()
        {
            SetEventHandler(0);
        }
        
        void AWindow::SetEventHandler(WindowEventHandler* handler)
        {
            if (handler != 0)
                handler->AddRef();

            if (handler != 0)
                handler->Release();
            eventHandler = handler;
        }
        WindowEventHandler* AWindow::GetEventHandler()
        {
            return eventHandler;
        }
        
        WindowImplementation AWindow::GetImplementation()
        {
            return WindowImplementationAndroid;
        }

        void AWindow::OnInitialize()
        {
            PrintLine("~~~~~~~~~~~~~~~~~ Window OnInitialize");
            context_.Initialize(Xli::GLContextAttributes::Default());
        }

        void AWindow::OnShow()
        {
            PrintLine("~~~~~~~~~~~~~~~~~ Window OnShow");
            context_.MakeCurrent(this);
            Xli::Application::SharedApp()->BecomeVisible();
        }

        void AWindow::OnHide()
        {
            PrintLine("~~~~~~~~~~~~~~~~~ Window OnHide");
        }

        void AWindow::OnDraw()
        {
            context_.SwapBuffers();
        }

        void AWindow::OnDestroy()
        {
            PrintLine("~~~~~~~~~~~~~~~~~ Window OnDestroy");
            // context_.Destroy();
            // [window_ release];

            // window_ = 0;
        }

        String AWindow::GetTitle()
        {
            return "";
        }
        void AWindow::SetTitle(const String& title) {}

        Vector2i AWindow::GetClientSize()
        {
            return Display::GetScreenSize();
        }
        void AWindow::SetClientSize(Vector2i size) {}

        Vector2i AWindow::GetPosition()
        {
            return Vector2i(0, 0); //{TODO} how does this relate to nativeui?
        }
        void AWindow::SetPosition(Vector2i pos) {}

        bool AWindow::IsFullscreen()
        {
            return true; //{TODO} need to query java for this
        }
        void AWindow::SetFullscreen(bool fullscreen) {}

        
        bool AWindow::IsMaximized()
        {
            return true;
        }
        bool AWindow::IsMinimized()
        {
            return false; //{TODO}
        }
        void AWindow::Maximize() {}
        void AWindow::Minimize() {}
        void AWindow::Restore() {}
        void AWindow::Close() {}
        bool AWindow::IsClosed() { return false; }

        int AWindow::GetDisplayIndex() { return 0; }

        void* AWindow::GetNativeHandle()
        {
            if (Xli::PlatformSpecific::AShim::SupportsNativeUI())
            {
                jobject unoSurface = AShim::GetUnoSurface();
                AJniHelper jni;
                return (void*)ANativeWindow_fromSurface(jni.GetEnv(), unoSurface);                    
            } else {
                return Xli::PlatformSpecific::AndroidApplication->window;
            }
        }
        GLContext* AWindow::GetContext() { return &context_; }
    }
    Window* Window::GetMainWindow()
    {
        return Xli::Application::SharedApp()->RootWindow();
    }
    void Window::SetMainWindow(Window* wnd) {}

    void InitWindow()
    {
    }

    void TerminateWindow()
    {
    }
}


// Window* Window::Create(int width, int height, const String& title, int flags)
// {
//     if (GlobalWindow != 0)
//         XLI_THROW("Only one window instance is allowed on the Android platform");

//     GlobalWidth = width;
//     GlobalHeight = height;
//     GlobalFlags = flags;
//     GlobalWindow = new PlatformSpecific::AWindow();

//     return GlobalWindow;
// }


// virtual void Close()
// {
//     if (GlobalAndroidApp->destroyRequested == 1 ||
//         (GlobalEventHandler != 0 &&
//          GlobalEventHandler->OnClosing(this)))
//         return;

//     GlobalAndroidApp->destroyRequested = 1;

//     if (GlobalEventHandler != 0)
//         GlobalEventHandler->OnClosed(this);
// }

// virtual SystemCursor GetSystemCursor()
// {
//     return SystemCursorNone;
// }

// virtual void SetSystemCursor(SystemCursor cursor)
// {
// }
