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

#ifndef __XLI_CTKEY_ANDROID_H__
#define __XLI_CTKEY_ANDROID_H__

#include "AKeyEvent.h"
#include <Xli/MutexQueue.h>

namespace Xli
{
    namespace PlatformSpecific
    {
        class WindowAction : public Object
        {
        public:
            virtual void Execute() = 0;
        };

        class CTKeyAction : public WindowAction
        {
        public:
            Xli::Key KeyEvent;
            bool KeyDown;
            CTKeyAction(AKeyEvent keyEvent, bool keyDown)
            {
                this->KeyEvent = AndroidToXliKeyEvent(keyEvent);
                this->KeyDown = keyDown;
            }
            virtual void Execute();
        };

        class CTTextAction : public WindowAction
        {
        public:
            String Text;
            CTTextAction(String text) { this->Text = text; }
            virtual void Execute();
        };

        class CTKeyboardResize : public WindowAction
        {
        public:
            CTKeyboardResize() {}
            virtual void Execute();
        };

        class CTError : public WindowAction
        {
        public:
            String message;
            int errorCode;
            CTError(String message, int errorCode)
            {
                this->message = message;
                this->errorCode = errorCode;
            }
            virtual void Execute();
        };

        class CTQueue
        {
        public:
            static MutexQueue<WindowAction*> ctActionQueue;
            static void EnqueueCrossThreadEvent(WindowAction* action);
            static void ProcessCrossThreadEvents();
        };
    };
};

#endif
