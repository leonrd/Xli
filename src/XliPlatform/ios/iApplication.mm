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

#include <XliPlatform/Application.h>
#include <XliPlatform/Display.h>
#include <XliPlatform/Window.h>
#include <XliPlatform/PlatformLib.h>
#include <Xli/Managed.h>
#include <Xli/Thread.h>
#include <Xli/Time.h>
#include "iWindow.h"
#include "AppDelegate.h"

#include <Xli/Console.h>

namespace Xli
{
    static PlatformSpecific::iWindow window_;
    static CADisplayLink* displayLink_ = 0;

    int Application::Run(int argc, char** argv)
    {
        // Notes on exception handling (iOS, 64-bit):
        //  - full interoperability between Objective C and C++ exceptions
        //  - std::set_terminate will replace Objective C's uncaught exception
        //  handler, typically returned as the previous handler.
        //  - try is free, throw is expensive

        // SDL's main only calls UIApplicationMain, SDL_main called from it's
        // application delegate after didFinishLaunchingWithOptions.

        // Need the pool, but little point in actually releasing it
        (void) [[NSAutoreleasePool alloc] init];
        
        // UIApplicationMain doesn't return, but exceptions may be caught here.
        return UIApplicationMain(
            argc, argv, nil, NSStringFromClass([uObjC_AppDelegate class]));
    }

    Window* Application::RootWindow()
    {
        return &window_;
    }

    unsigned Application::FrameRate() const
    {
        if (!displayLink_ || displayLink_.paused)
            return 0;
        return 1.0 / (displayLink_.frameInterval * displayLink_.duration);
    }

    void Application::SetFrameRate(unsigned frameRate)
    {
        if (!displayLink_)
            return;

        displayLink_.frameInterval
            = 1.0 / frameRate / displayLink_.duration;
    }

    // void OnUpdateFrame() {}

    void Application::EmitOnStart()
    {
        PrintLine("----------------- EmitOnStart");
        window_.Initialize();
        window_.SetEventHandler(this);

        uObjC_AppDelegate* appDelegate = [UIApplication sharedApplication].delegate;

        displayLink_ = [CADisplayLink displayLinkWithTarget:appDelegate
                        selector:@selector(uObjC_OnUpdateFrame:)];
        [displayLink_ addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
        displayLink_.paused = YES;
        OnStart();
    }

    void Application::EmitOnDidStart()
    {
        PrintLine("----------------- EmitOnDidStart");
        OnDidStart();
    }

    void Application::EmitOnEnterVisible()
    {
        PrintLine("----------------- EmitOnEnterVisible");
        window_.Show();

        displayLink_.frameInterval = 10;
        displayLink_.paused = NO;

        OnEnterVisible();
    }

    void Application::EmitOnEnterActive()
    {
        PrintLine("----------------- EmitOnEnterActive");
        displayLink_.frameInterval = 1;
        OnEnterActive();
    }

    void Application::EmitOnExitActive()
    {
        PrintLine("----------------- EmitOnExitActive");
        OnExitActive();
        displayLink_.frameInterval = 10;
    }

    void Application::EmitOnEnterBackground()
    {
        PrintLine("----------------- EmitOnEnterBackground");
        OnEnterBackground();
        displayLink_.paused = YES;
        window_.Hide();
    }

    void Application::EmitOnTerminate()
    {
        PrintLine("----------------- EmitOnTerminate");
        OnTerminate();
        _FreeResources();
    }

    void Application::EmitOnLowMemory()
    {
        PrintLine("----------------- EmitOnLowMemory");
        OnLowMemory();
        if (window_.CurrentState() == Window::Hidden)
            _FreeResources();
    }

    void Application::_FreeResources()
    {
        [displayLink_ invalidate];
        displayLink_ = 0;

        window_.Destroy();
    }
}
