/* * Copyright (C) 2010-2014 Outracks Technologies
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __XLI_PLATFORM_WINDOW_H__
#define __XLI_PLATFORM_WINDOW_H__

#include <Xli/Object.h>
#include <Xli/Vector2.h>
#include <XliPlatform/EventHandler.h>

namespace Xli
{
    /**
        \ingroup XliPlatform

        Represents a window in the native window system.
    */
    class Window: public Object
    {
    public:
        virtual ~Window() {}

        /**
            Returns the window implementation type
        */
        virtual WindowImplementation GetImplementation() = 0;

        /**
            Sets the handler that should recieve events from the window
        */
        virtual void SetEventHandler(WindowEventHandler* handler) = 0;

        /**
            Returns the current event handler for the window
        */
        virtual WindowEventHandler* GetEventHandler() = 0;

        /**
            Closes the window
        */
        virtual void Close() = 0;

        /**
            Returns wether or not the window has been closed by the user or operating system.
        */
        virtual bool IsClosed() = 0;

        /**
            Returns wether or not the window is currently visible (i.e. not minimized or hidden).
        */
        virtual bool IsVisible() = 0;

        /**
            Sets the fullscreen mode of the window. 
            The window will go fullscreen on the display which contains the windows centre coordinate and stretched to that displays resolution.
        */
        virtual void SetFullscreen(bool fullscreen) = 0;

        /**
            Returns if this is a fullscreen window
        */
        virtual bool IsFullscreen() = 0;

        /**
            Minimizes the window
        */
        virtual void Minimize() = 0;

        /**
            Maximizes the window
        */
        virtual void Maximize() = 0;

        /**
            Restores the window from maximized/minimized state to normal position
        */
        virtual void Restore() = 0;

        /**
            Returns if this window is minimized
        */
        virtual bool IsMinimized() = 0;

        /**
            Returns if this window is maximized
        */
        virtual bool IsMaximized() = 0;

        /**
            Sets the windows title
        */
        virtual void SetTitle(const String& title) = 0;

        /**
            Returns the windows title
        */
        virtual String GetTitle() = 0;

        /**
            Sets the position of the window
        */
        virtual void SetPosition(Vector2i pos) = 0;

        /**
            Returns the position of the window
        */
        virtual Vector2i GetPosition() = 0;

        /**
            Sets the size of the client area of the window
        */
        virtual void SetClientSize(Vector2i size) = 0;

        /**
            Returns the size of the client area of the window
        */
        virtual Vector2i GetClientSize() = 0;

        /**
            Returns the index for the display containing this windows center coordinate
        */
        virtual int GetDisplayIndex() = 0;

        /**
            Returns the native windows handle
        */
        virtual void* GetNativeHandle() = 0;

        /**
            Returns true if the specified Key is currently pressed
        */
        virtual bool GetKeyState(Key key) = 0;

        /**
            Returns true if the specified MouseButton is currently pressed
        */
        virtual bool GetMouseButtonState(MouseButton button) = 0;

        /**
            Sets the current mouse position relative to the client area of the window
        */
        virtual void SetMousePosition(Vector2i position) = 0;

        /**
            Returns the current mouse position relative to the client area of the window
        */
        virtual Vector2i GetMousePosition() = 0;

        /**
            Sets the system cursor to be used in the window
        */
        virtual void SetSystemCursor(SystemCursor cursor) = 0;

        /**
            Returns the current system cursor used in the window
        */
        virtual SystemCursor GetSystemCursor() = 0;

        virtual void BeginTextInput(TextInputHint hint) { }
        virtual void EndTextInput() { }
        virtual bool IsTextInputActive() { return false; }

        virtual bool HasOnscreenKeyboardSupport() { return false; }
        virtual bool IsOnscreenKeyboardVisible() { return false; }

        virtual void SetOnscreenKeyboardPosition(Vector2i position) { }
        virtual Vector2i GetOnscreenKeyboardPosition() { return Vector2i(0, 0); }
        virtual Vector2i GetOnscreenKeyboardSize() { return Vector2i(0, 0); }

        virtual bool IsStatusBarVisible() { return false; }
        virtual Vector2i GetStatusBarPosition() { return Vector2i(0, 0); }
        virtual Vector2i GetStatusBarSize() { return Vector2i(0, 0); }

        /**
            Sets the window that should be used as main window
        */
        static void SetMainWindow(Window* wnd);

        /**
            Returns the first window created or the window currently set using SetMainWindow().
            If no window is created or the main window is destroyed this function will return NULL.
        */
        static Window* GetMainWindow();

        /**
            Returns the size of the primary monitor
        */
        static Vector2i GetScreenSize();

        /**
            Creates a window
        */
        static Window* Create(int width, int height, const Xli::String& title, int flags = 0);

        /**
            Creates a window
        */
        static Window* Create(const Vector2i& size, const Xli::String& title, int flags = 0)
        { 
            return Create(size.X, size.Y, title, flags);
        }

        /**
            Adopts a native window
        */
        static Window* CreateFrom(void* nativeWindowHandle);

        /**
            Process messages for the application to keep user interface responsive.
        */
        static void ProcessMessages();
    };
}

#endif
