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
#include <XliPlatform/Window.h>
#include <Xli/Console.h>
#include "AJniHelper.h"
#include "AWindow.h"
#include "AShim.h"

static int GlobalFlags = 0;

namespace Xli
{
    namespace PlatformSpecific
    {
        AWindow::AWindow()
        {
            // int nativeFlags = 0;
            // if (GlobalFlags & WindowFlagsFullscreen)
            //     nativeFlags |= AWINDOW_FLAG_FULLSCREEN;
            // if (GlobalFlags & WindowFlagsDisablePowerSaver)
            //     nativeFlags |= AWINDOW_FLAG_KEEP_SCREEN_ON | AWINDOW_FLAG_TURN_SCREEN_ON;
            //ANativeActivity_setWindowFlags(GlobalAndroidApp->activity, nativeFlags, 0);
        }

        AWindow::~AWindow()
        {
            SetEventHandler(0);
        }

        Window::State AWindow::CurrentState() const { return state_; }
        
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
        
        WindowImplementation GetImplementation()
        {
            return WindowImplementationAndroid;
        }

        void AWindow::OnInitialize()
        {
            //{TODO} need to subscribe to the android window events            
        }

        void AWindow::OnShow()
        {
            context_.MakeCurrent(0);
        }

        void AWindow::OnHide()
        {
        }

        void AWindow::OnDraw()
        {
            context_.SwapBuffers();
        }

        void AWindow::OnDestroy()
        {
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
            return Vector2i(GlobalWidth, GlobalHeight);
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
                return GlobalAndroidApp->window;
            }
        }
        GLContext* AWindow::GetContext() { return &context_; }

        Window* Window::GetMainWindow()
        {
            return GlobalWindow;
        }
        void Window::SetMainWindow(Window* wnd) {}
    }
}

    // void InitWindow()
    // {
    // }

    // void TerminateWindow()
    // {
    // }

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

// Window* Window::CreateFrom(void* nativeWindowHandle)
// {
//     XLI_THROW_NOT_SUPPORTED(XLI_FUNCTION);
// }

// virtual void SetEventHandler(InputEventHandler* handler)
// {
//     if (handler != 0)
//         handler->AddRef();

//     if (GlobalEventHandler != 0)
//         GlobalEventHandler->Release();

//     GlobalEventHandler = handler;
// }

// virtual InputEventHandler* GetEventHandler()
// {
//     return GlobalEventHandler;
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


// virtual void ShowCursor(bool show)
// {
// }

// virtual void BeginTextInput(TextInputHint hint)
// {
//     AShim::RaiseSoftKeyboard();
// }

// virtual void EndTextInput()
// {
//     AShim::HideSoftKeyboard();
// }

// virtual bool IsTextInputActive()
// {
//     return AShim::KeyboardVisible();
// }

// virtual bool HasOnscreenKeyboardSupport()
// {
//     return true;
// }

// virtual bool IsOnscreenKeyboardVisible()
// {
//     return AShim::KeyboardVisible();
// }

// virtual Vector2i GetOnscreenKeyboardPosition()
// {
//     return Vector2i(0, GlobalHeight - AShim::GetKeyboardSize());
// }

// virtual Vector2i GetOnscreenKeyboardSize()
// {
//     return Vector2i(GlobalWidth, AShim::GetKeyboardSize());
// }

// virtual bool GetKeyState(Key key)
// {
//     return false;
// }

// virtual bool GetMouseButtonState(MouseButton button)
// {
//     return false;
// }

// virtual Vector2i GetMousePosition()
// {
//     return Vector2i(0, 0);
// }

// virtual void SetMousePosition(Vector2i position)
// {
// }

// virtual SystemCursor GetSystemCursor()
// {
//     return SystemCursorNone;
// }

// virtual void SetSystemCursor(SystemCursor cursor)
// {
// }
