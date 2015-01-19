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

#ifndef __XLI_PLATFORM_SPECIFIC_ANDROID_INTERNAL_H__
#define __XLI_PLATFORM_SPECIFIC_ANDROID_INTERNAL_H__

#include <android/log.h>
#include <android/native_activity.h>
#include <cstdio>
#include <cstdlib>
#include <jni.h>

#include <XliPlatform/PlatformSpecific/Android.h>
#include <XliPlatform/Application.h>
#include <XliPlatform/Window.h>

#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, Xli::PlatformSpecific::AGetAppName(), __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, Xli::PlatformSpecific::AGetAppName(), __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, Xli::PlatformSpecific::AGetAppName(), __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, Xli::PlatformSpecific::AGetAppName(), __VA_ARGS__))
#define LOGF(...) ((void)__android_log_print(ANDROID_LOG_FATAL, Xli::PlatformSpecific::AGetAppName(), __VA_ARGS__))

struct android_app;

namespace Xli
{
    typedef int JObjRef;

    namespace PlatformSpecific
    {
        extern ANativeActivity* AndroidActivity;
        static struct android_app* AndroidApplication;

        XLI_INLINE const char* AGetAppName()
        {
            const char* name = getenv("XLI_APP_NAME");
            return name ? name : "XliApp";
        }

        class CTAction : public Object
        {
        public:
            virtual void Execute() = 0;
        };

        class CTSurfaceSizeChanged : public CTAction
        {
        public:
            virtual void Execute()
            {
                Application* app = Xli::Application::SharedApp();
                app->OnSizeChanged(app->RootWindow());
            }
        };
        
        class CTSurfaceReady : public CTAction
        {
        public:
            virtual void Execute()
            {
                // give to context via window
                Window* window = Application::SharedApp()->RootWindow();
                Application::SharedApp()->OnNativeHandleChanged(window);
                window->Show();
            }
        };
        
        class CTError : public CTAction
        {
        public:
            String message;
            int errorCode;
            CTError(String message, int errorCode)
            {
                this->message = message;
                this->errorCode = errorCode;
            }
            virtual void Execute()
            {
                String finalMessage = "XLiError (" + String(errorCode)+ ") - " + message;
                XLI_THROW(finalMessage.Ptr());
            }
        };
    }
}


#endif
