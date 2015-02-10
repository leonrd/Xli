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
#include <Xli/CoreLib.h>
#include <XliPlatform/PlatformSpecific/Android.h>
#include <Xli/MutexQueue.h>
#include <Xli/Console.h>
#include <XliGL/GLContext.h>

#include "AInternal.h"
#include "AJniHelper.h"
#include "AShim.h"
#include "AKeyEvent.h"
#include "ALogStream.h"

static Xli::MutexQueue<Xli::PlatformSpecific::CTAction*> cross_thread_event_queue;
static bool seperateCoreThread = false;

namespace Xli
{
    namespace PlatformSpecific
    {
        void Android::SetLogTag(const char* tag)
        {
            setenv("XLI_APP_NAME", tag, 1);
        }

        JavaVM* Android::GetJavaVM()
        {
            return AJniHelper::GetVM();
        }

        jobject Android::GetActivity()
        {
            return AJniHelper::GetActivity();
        }

        void Android::ProcessCrossThreadEvents()
        {
            while ((cross_thread_event_queue.Count() > 0))
            {
                CTAction* action = cross_thread_event_queue.Dequeue();
                action->Execute();
                delete action;
            }
        }

        void Android::OnJNILoad(JavaVM* vm, JNIEnv* env, jclass shim_class)
        {
            Xli::PlatformSpecific::AJniHelper::Init(vm, env, shim_class);
        }

        void Android::Init()
        {
            Out->SetStream(ManagePtr(new ALogStream(ANDROID_LOG_INFO)));
            Error->SetStream(ManagePtr(new ALogStream(ANDROID_LOG_WARN)));
        }
    }
}
