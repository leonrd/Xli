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


#include <XliPlatform/InputEventHandler.h> 
#include <XliPlatform/Window.h>
#include <Xli/Thread.h>
#include <Xli/StringBuilder.h>
#include <cstring>
#include <cassert>

namespace Xli
{
    void Window::Initialize()
    {
        switch (state_)
        {
        case Visible:
            assert(!"Invalid state transition");
            Hide();

        case Hidden:
            assert(!"Invalid state transition");
            Destroy();

        case Destroying:
            state_ = Initializing;
            OnInitialize();

        case Initializing:
            // On it!
            break;
        }
    }

    void Window::Show()
    {
        switch (state_)
        {
        case Destroying:
            assert("Invalid state transition");
            Initialize();

        case Initializing:
        case Hidden:
            OnShow();

        case Visible:
            // On it!
            break;
        }
    }

    void Window::Hide()
    {
        switch (state_)
        {
        case Destroying:
            assert("Invalid state transition");
            Initialize();
            return Hide();

        case Visible:
            OnHide();

        case Initializing:
            state_ = Hidden;

        case Hidden:
            // On it!
            break;
        }
    }

    void Window::Destroy()
    {
        switch (state_)
        {
        case Visible:
            Hide();

        case Initializing:
        case Hidden:
            state_ = Destroying;
            OnDestroy();

        case Destroying:
            // On it!
            break;
        }
    }
}
