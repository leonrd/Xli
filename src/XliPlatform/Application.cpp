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

    void Application::EnterForeground()
    {
        PrintLine("----------------- EnterForeground");
        switch (state_)
        {
        case Terminating:
            XLI_THROW("EnterForeground() called on terminating Application");

        case Uninitialized:
            Start();

        case Starting:
            EmitOnDidStart();

        case Background:
            state_ = Foreground;
            EmitOnEnterForeground();
            break;

        case Interactive:
            // Sub-state of Foreground.
        case Foreground:
            // On it!
            break;
        }

        assert(state_ == Foreground
            || state_ == Interactive);
    }

    void Application::EnterInteractive()
    {
        PrintLine("----------------- EnterInteractive");
        switch (state_)
        {
        case Terminating:
            XLI_THROW("EnterInteractive() called on terminating Application");

        case Uninitialized:
        case Starting:
        case Background:
            EnterForeground();

        case Foreground:
            state_ = Interactive;
            EmitOnEnterInteractive();

        case Interactive:
            // On it!
            break;
        }

        assert(state_ == Interactive);
    }

    void Application::ExitInteractive()
    {
        PrintLine("----------------- ExitInteractive");

        if (state_ != Interactive)
            return;

        state_ = Foreground;
        EmitOnExitInteractive();

        assert(state_ == Foreground);
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

        case Interactive:
            // Harmless if not Interactive
            ExitInteractive();

        case Foreground:
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

        case Interactive:
        case Foreground:
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
