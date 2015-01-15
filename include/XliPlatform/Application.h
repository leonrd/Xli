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

#ifndef __XLI_APPLICATION_H__
#define __XLI_APPLICATION_H__

#include <XliPlatform/Window.h>
#include <XliPlatform/InputEventHandler.h>

namespace Xli
{
    class Application: public InputEventHandler, public WindowEventHandler
    {
    public:
        static Application* SharedApp();

        enum State
        {
            Terminating,
            Starting,
            Visible,
            Active,
            Background,
        };

        virtual int Run(int argc, char **argv);

        char const *GetTitle() const;
        State CurrentState() const { return state_; }

        void Start();
        void BecomeVisible();
        void BecomeActive();
        void ResignActive();
        void EnterBackground();
        void Terminate();

        virtual Window* RootWindow();

        virtual unsigned FrameRate() const;
        virtual void SetFrameRate(unsigned frameRate);

        virtual String GetInitTitle();
        virtual Vector2i GetInitSize();

        virtual void OnUpdateFrame() {}        
        virtual void OnLowMemory() {}      
    protected:
        virtual void OnStart() {}
        virtual void OnDidStart() {}
        virtual void OnEnterVisible() {}
        virtual void OnEnterActive() {}
        virtual void OnExitActive() {}
        virtual void OnEnterBackground() {}
        virtual void OnTerminate() {}


        Application() : state_(Terminating) {}
        virtual ~Application() {}

    private:
        virtual void _FreeResources();
        Application(Application const &);
        void operator=(Application const &);

        void EmitOnStart();
        void EmitOnDidStart();
        void EmitOnEnterVisible();
        void EmitOnEnterActive();
        void EmitOnExitActive();
        void EmitOnEnterBackground();
        void EmitOnTerminate();
        void EmitOnLowMemory();

        State state_;
    };
}

#endif
