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

#include <XliPlatform/Display.h>
#include <XliPlatform/Window.h>
#include <Xli/Array.h>

namespace Xli
{
    int Display::GetCount()
    {
        return 1;
    }

    Vector2i Display::GetScreenSize() { return Vector2i(0, 0); }
    Recti Display::GetRect(int index)
    {
        Vector2i size = Display::GetScreenSize();
        return Recti(0, 0, size.X, size.Y);
    }

    bool Display::GetCurrentSettings(int index, DisplaySettings& settings)
    {
        return false;
    }

    void Display::GetSupportedSettings(int index, Array<DisplaySettings>& settings)
    {
    }

    bool Display::ChangeSettings(int index, const DisplaySettings& settings)
    {
        return false;
    }

    float Display::GetDensity(int displayIndex)
    {
        // TODO
        return 1.0f;
    }
    Vector2 Display::GetDpi(int displayIndex)
    {
        // TODO
        return Vector2(72, 72);
    }

    bool Display::IsStatusBarVisible()
    {
        return true;
    }

    Vector2i Display::GetStatusBarPosition()
    {
        return Vector2i(0, 0);
    } 
    Vector2i Display::GetStatusBarSize()
    {
        return Vector2i(0,0);
    }
}
