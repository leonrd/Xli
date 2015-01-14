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

#include <Xli.h>
#include <XliGL/GLContext.h>
#include <XliPlatform/Application.h>
#include <XliPlatform/Display.h>
#include <XliPlatform/Window.h>
#include <XliPlatform/InputEventHandler.h>

#include "AppDelegate.h"
#include "iWindow.h"

#include <UIKit/UIKit.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <QuartzCore/QuartzCore.h>

@interface uObjC_GLView : UIView
@end

@implementation uObjC_GLView
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}
@end


@interface uObjC_GLViewController : UIViewController
@end

@implementation uObjC_GLViewController
@end

Xli::Window* uStatic__Window;
Xli::GLContext* uStatic__GLContext;

namespace Xli
{
    namespace PlatformSpecific
    {
        iWindow::iWindow()
        {
            state_ = Destroying;
            window_ = 0;
        }
        
        iWindow::~iWindow()
        {
            [window_ release];
        }

        void iWindow::OnInitialize()
        {
            assert(window_ == 0);

            CGRect screenBounds = [[UIScreen mainScreen] bounds];

            window_ = [[UIWindow alloc] initWithFrame:screenBounds];
            window_.rootViewController
                = [[[uObjC_GLViewController alloc] init] autorelease];
            window_.rootViewController.view = [[[uObjC_GLView alloc]
                                                initWithFrame:screenBounds] autorelease];

            context_.Initialize(
                (CAEAGLLayer *) window_.rootViewController.view.layer);

            window_.hidden = false;
            [window_ makeKeyWindow];

            uStatic__Window = this;
            uStatic__GLContext = &context_;
        }

        void iWindow::OnShow()
        {
            context_.MakeCurrent(0);
        }

        // void OnHide() {}

        void iWindow::OnDraw()
        {
            context_.SwapBuffers();
        }

        void iWindow::OnDestroy()
        {
            context_.Destroy();
            [window_ release];

            window_ = 0;
        }

        void iWindow::Close() {}
        bool iWindow::IsClosed() { return false; }
        String iWindow::GetTitle() {}
        Vector2i iWindow::GetClientSize() { return Vector2i(0, 0);}
        Vector2i iWindow::GetPosition() { return Vector2i(0, 0); }
        bool iWindow::IsFullscreen() {}
        bool iWindow::IsMaximized() {}
        bool iWindow::IsMinimized() {}
        int iWindow::GetDisplayIndex() { return 0; }
        void iWindow::Maximize() {}
        void iWindow::Minimize() {}
        void iWindow::Restore() {}
        void iWindow::SetClientSize(Vector2i size) {}
        void iWindow::SetFullscreen(bool fullscreen) {}
        void iWindow::SetPosition(Vector2i pos) {}
        void iWindow::SetTitle(const String& title) {}
        void* iWindow::GetNativeHandle() { return (void*)0; }
    }

    
    Vector2i Window::GetScreenSize() { return Vector2i(0, 0); }

    Window* Window::GetMainWindow() { return (Window*)0; }

    void Window::SetMainWindow(Window* wnd) {}
}
