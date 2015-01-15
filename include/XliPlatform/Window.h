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
#include <XliPlatform/WindowEventHandler.h>

namespace Xli
{

    class GLContext;
    
    class Window : public Object
    {
    public:        
        enum State
        {
            Destroying,
            Initializing,
            Visible,
            Hidden,
        };

        Window()
        {
            state_ = Destroying;
        }
        
        virtual ~Window() {}

        State CurrentState() const { return state_; }

        void Initialize();
        void Show();
        void Hide();
        void Destroy();

        virtual void OnDraw() {}

        /**
            Returns the window implementation type (enum)
        */        
        WindowImplementation GetImplementation();

        /**
           Returns the window implementation type (enum)
        */        
        virtual void SetEventHandler(WindowEventHandler* handler);
        virtual WindowEventHandler* GetEventHandler();

        //{TODO}
        /**
           Closes the window
        */
        virtual void Close();

        /**
           Returns wether or not the window has been closed by the user or operating system.
        */
        virtual bool IsClosed();
        
        /**
            Sets the fullscreen mode of the window. 
            The window will go fullscreen on the display which contains the windows centre coordinate and stretched to that displays resolution.
        */
        virtual void SetFullscreen(bool fullscreen);

        /**
            Returns if this is a fullscreen window
        */
        virtual bool IsFullscreen();

        /**
            Minimizes the window
        */
        virtual void Minimize();

        /**
            Maximizes the window
        */
        virtual void Maximize();

        /**
            Restores the window from maximized/minimized state to normal position
        */
        virtual void Restore();

        /**
            Returns if this window is minimized
        */
        virtual bool IsMinimized();

        /**
            Returns if this window is maximized
        */
        virtual bool IsMaximized();

        /**
            Sets the windows title
        */
        virtual void SetTitle(const String& title);

        /**
            Returns the windows title
        */
        virtual String GetTitle();

        /**
            Sets the position of the window
        */
        virtual void SetPosition(Vector2i pos);

        /**
            Returns the position of the window
        */
        virtual Vector2i GetPosition();

        /**
            Sets the size of the client area of the window
        */
        virtual void SetClientSize(Vector2i size);

        /**
            Returns the size of the client area of the window
        */
        virtual Vector2i GetClientSize();

        /**
            Returns the index for the display containing this windows center coordinate
        */
        virtual int GetDisplayIndex();

        /**
            Returns the native windows handle
        */
        virtual void* GetNativeHandle();

        /**
           Returns the Window's GLContext
        */        
        virtual GLContext* GetContext() {}
        
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

    protected:
        virtual void OnInitialize() {}
        virtual void OnShow() {}
        virtual void OnHide() {}
        virtual void OnDestroy() {}

    private:        
        Window(Window const &);
        void operator=(Window const &);

        State state_;
    };

}

#endif
