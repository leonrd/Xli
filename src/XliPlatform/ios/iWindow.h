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

#ifndef __XLI_PLATFORM_SPECIFIC_IOS_WINDOW_H__
#define __XLI_PLATFORM_SPECIFIC_IOS_WINDOW_H__

#include <XliPlatform/InputEventHandler.h>
#include <XliPlatform/Window.h>
#include <Xli/Shared.h>
#include <XliGL/GLContext.h>
#include "iGLContext.h"

namespace Xli
{
    namespace PlatformSpecific
    {
        class iWindow : public Window
        {
        public:        
            iWindow(int width, int height, const String& title, int flags);
        
            virtual ~iWindow();

            State CurrentState() const { return state_; }

            void Initialize();
            void Show();
            void Hide();
            void Destroy();

            virtual void OnDraw();

            virtual String GetTitle();
            virtual Vector2i GetClientSize();
            virtual Vector2i GetPosition();
            virtual WindowImplementation GetImplementation();
            virtual bool IsClosed();
            virtual bool IsFullscreen();
            virtual bool IsMaximized();
            virtual bool IsMinimized();
            virtual int GetDisplayIndex();
            virtual void Close();
            virtual void Maximize();
            virtual void Minimize();
            virtual void Restore();
            virtual void SetClientSize(Vector2i size);
            virtual void SetFullscreen(bool fullscreen);
            virtual void SetPosition(Vector2i pos);
            virtual void SetTitle(const String& title);
            virtual void* GetNativeHandle();

        protected:
            virtual void OnInitialize();
            virtual void OnShow();
            virtual void OnHide();
            virtual void OnDestroy();

        private:
            iGLContext context_;
            UIWindow* window_;
            bool _fullscreen;
            
            iWindow(iWindow const &);
            void operator=(iWindow const &);

            State state_;
        };        
    }
}


#endif
