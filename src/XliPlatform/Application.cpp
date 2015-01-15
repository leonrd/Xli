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
#include <XliPlatform/PlatformLib.h>
#include <Xli/Managed.h>
#include <Xli/Thread.h>
#include <Xli/Time.h>
#include <cassert>

#include <Xli/Console.h>

namespace Xli
{
    
    String Application::GetInitTitle()
    {
        return "Xli Application";
    }

    Vector2i Application::GetInitSize()
    {
#if defined(XLI_PLATFORM_IOS) || defined(XLI_PLATFORM_ANDROID)
        return Window::GetScreenSize();
#else
        return Vector2i(1280, 720);
#endif
    }

    // Lifecycle    
    void Application::Start()
    {
        PrintLine("----------------- Start");
        switch (state_)
        {
        case Active:
        case Visible:
        case Background:
            assert("Invalid state transition");

            Terminate();

        case Terminating:
            state_ = Starting;
            EmitOnStart();

        case Starting:
            // On it!
            break;
        }
    }

    void Application::BecomeVisible()
    {
        PrintLine("----------------- BecomeVisible");        
        switch (state_)
        {
        case Terminating:
            assert("Invalid state transition");

            Start();

        case Starting:
            EmitOnDidStart();

        case Background:
            state_ = Visible;
            EmitOnEnterVisible();
            break;

        case Active:
            ResignActive();

        case Visible:
            // On it!
            break;
        }
    }

    void Application::BecomeActive()
    {
        PrintLine("----------------- BecomeActive");
        switch (state_)
        {
        case Terminating:
            assert(!"Invalid state transition");

            Start();

        case Background:
        case Starting:
            BecomeVisible();

        case Visible:
            EmitOnEnterActive();
            state_ = Active;

        case Active:
            break;
        }
    }

    void Application::ResignActive()
    {
        PrintLine("----------------- ResignActive");
        switch (state_)
        {
        case Terminating:
            assert(!"Invalid state transition");

            Start();

        case Starting:
        case Background:
            assert(!"Invalid state transition");

            BecomeVisible();
            break;

        case Active:
        case Visible:
            state_ = Visible;
            EmitOnExitActive();
            break;
        }
    }

    void Application::EnterBackground()
    {
        PrintLine("----------------- EnterBackground");
        switch (state_)
        {
        case Active:
            ResignActive();

        case Visible:
        case Starting:
            state_ = Background;
            EmitOnEnterBackground();
            break;

        case Terminating:
            assert(!"Invalid state transition");

            Start();
            return EnterBackground();

        case Background:
            // On it!
            break;
        }
    }

    void Application::Terminate()
    {
        PrintLine("----------------- Terminate");
        switch (state_)
        {
        case Active:
            ResignActive();

        case Visible:
        case Starting:
            EnterBackground();

        case Background:
            EmitOnTerminate();

        case Terminating:
            // On it!
            break;
        }
    }
}
