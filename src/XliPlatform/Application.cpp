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
#include <XliPlatform/Window.h>
#include <XliPlatform/Display.h>

#include <Xli/Console.h>
#include <Xli/Exception.h>

#include <cassert>

namespace Xli
{
    Application* Application::application_;
    
    String Application::GetInitTitle()
    {
        return "Xli Application";
    }

    Vector2i Application::GetInitSize()
    {
#if defined(XLI_PLATFORM_IOS) || defined(XLI_PLATFORM_ANDROID)
        return Display::GetScreenSize();
#else
        return Vector2i(1280, 720);
#endif
    }

    // Lifecycle    
    void Application::Start()
    {
        PrintLine("----------------- Start");

        if (state_ != Uninitialized)
            XLI_THROW("Start() called on running Application");

        state_ = Starting;
        EmitOnStart();

        assert(state_ == Starting);
    }

    void Application::BecomeVisible()
    {
        PrintLine("----------------- BecomeVisible");        
        switch (state_)
        {
        case Terminating:
            XLI_THROW("BecomeVisible() called on terminating Application");

        case Uninitialized:
            Start();

        case Starting:
            EmitOnDidStart();

        case Background:
            state_ = Visible;
            EmitOnEnterVisible();
            break;

        case Active:
            // Sub-state of Visible
        case Visible:
            // On it!
            break;
        }

        assert(state_ == Active
            || state_ == Visible);
    }

    void Application::BecomeActive()
    {
        PrintLine("----------------- BecomeActive");
        switch (state_)
        {
        case Terminating:
            XLI_THROW("BecomeActive() called on terminating Application");

        case Uninitialized:
        case Background:
        case Starting:
            BecomeVisible();

        case Visible:
            state_ = Active;
            EmitOnEnterActive();

        case Active:
            // On it!
            break;
        }

        assert(state_ == Active);
    }

    void Application::ResignActive()
    {
        PrintLine("----------------- ResignActive");

        if (state_ != Active)
            return;

        state_ = Visible;
        EmitOnExitActive();

        assert(state_ == Visible);
    }

    void Application::EnterBackground()
    {
        PrintLine("----------------- EnterBackground");
        switch (state_)
        {
        case Terminating:
            XLI_THROW("EnterBackground() called on terminating Application");

        case Uninitialized:
            Start();

        case Active:
            // Harmless, if not active
            ResignActive();

        case Visible:
        case Starting:
            state_ = Background;
            EmitOnEnterBackground();

        case Background:
            // On it!
            break;
        }

        assert(state_ == Background);
    }

    void Application::Terminate()
    {
        PrintLine("----------------- Terminate");
        switch (state_)
        {
        case Uninitialized:
            // No point in initializing now.
            break;

        case Active:
            ResignActive();

        case Visible:
        case Starting:
            EnterBackground();

        case Background:
            state_ = Terminating;
            EmitOnTerminate();

        case Terminating:
            // On it!
            break;
        }

        assert(state_ == Uninitialized
            || state_ == Terminating);
    }
}
