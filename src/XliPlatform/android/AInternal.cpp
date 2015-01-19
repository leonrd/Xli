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

#include <XliPlatform/PlatformSpecific/Android.h>
#include <XliPlatform/Application.h>
#include <Xli/MutexQueue.h>
#include <Xli/Console.h>
#include <XliGL/GLContext.h>

#include "../../../3rdparty/android_native_app_glue/android_native_app_glue.h"
#include "AInternal.h"
#include "AShim.h"
#include "AKeyEvent.h"
#include "ALogStream.h"

static Xli::MutexQueue<Xli::PlatformSpecific::CTAction*> cross_thread_event_queue;
// static Xli::Application* application;

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
            return AndroidActivity->vm;
        }

        jobject Android::GetActivity()
        {
            return AndroidActivity->clazz;
        }

        static const char* get_cmd_string(int32_t cmd)
        {
            switch (cmd)
            {
#define CASE(x) case x: return #x;
                CASE(APP_CMD_INPUT_CHANGED);
                CASE(APP_CMD_INIT_WINDOW);
                CASE(APP_CMD_TERM_WINDOW);
                CASE(APP_CMD_WINDOW_RESIZED);
                CASE(APP_CMD_WINDOW_REDRAW_NEEDED);
                CASE(APP_CMD_CONTENT_RECT_CHANGED);
                CASE(APP_CMD_GAINED_FOCUS);
                CASE(APP_CMD_LOST_FOCUS);
                CASE(APP_CMD_CONFIG_CHANGED);
                CASE(APP_CMD_LOW_MEMORY);
                CASE(APP_CMD_START);
                CASE(APP_CMD_RESUME);
                CASE(APP_CMD_SAVE_STATE);
                CASE(APP_CMD_PAUSE);
                CASE(APP_CMD_STOP);
                CASE(APP_CMD_DESTROY);
#undef CASE
            }
            return "<UNKNOWN>";
        }

        void Android::ProcessMessages()
        {
            int ident;
            int events;
            struct android_poll_source* source;
            while ((ident = ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0)
            {
                if (source != NULL)
                {
                    source->process(Xli::PlatformSpecific::AndroidApplication, source);
                }
            }
        }
        
        static void handle_cmd(struct android_app* app, int32_t cmd)
        {
#ifdef XLI_DEBUG
            LOGD(get_cmd_string(cmd));
#endif
            switch (cmd)
            {
            case APP_CMD_START:
                break;
            case APP_CMD_RESUME:                
                break;
            case APP_CMD_PAUSE:
                Xli::Application::SharedApp()->ResignActive();
                break;
            case APP_CMD_STOP:
                Xli::Application::SharedApp()->EnterBackground();
                break;
            case (APP_CMD_GAINED_FOCUS):
                //Xli::Application::SharedApp()->BecomeActive();
                break;
            case (APP_CMD_LOST_FOCUS):
                break;
            case APP_CMD_DESTROY:
                Xli::Application::SharedApp()->Terminate();
                break;
            case APP_CMD_LOW_MEMORY:
                Xli::Application::SharedApp()->OnLowMemory();
                break;
            }
        }

        static int32_t handle_input(struct android_app* app, AInputEvent* event)
        {
            // We want to trap menubutton events but let the rest go to android
            // we will be capturing input and text events from java objects
            // if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY &&
            //     AKeyEvent_getKeyCode(event) == AKEYCODE_MENU)
            // {
            //     int32_t action = AKeyEvent_getAction(event);
            //     if (action == AKEY_EVENT_ACTION_DOWN) {
            //         return GlobalEventHandler->OnKeyDown(GlobalWindow, KeyMenu);
            //     } else if (action == AKEY_EVENT_ACTION_UP) {
            //         return GlobalEventHandler->OnKeyUp(GlobalWindow, KeyMenu);
            //     }
            // }
            return 0;
        }

        extern "C"
        {
            void JNICALL XliJ_JavaThrowError (JNIEnv* env , jobject obj, jint errorCode, jstring errorMessage) 
            {
                char const* cerrorMessage = env->GetStringUTFChars(errorMessage, NULL);
                String finalMessage = String("JavaThrown:") + String(cerrorMessage);
                cross_thread_event_queue.Enqueue(new CTError(finalMessage, errorCode));
                env->ReleaseStringUTFChars(errorMessage, cerrorMessage);
            }

            void JNICALL XliJ_UnoSurfaceReady (JNIEnv* env, jobject obj, jobject unoSurface)
            {
                // give to context via window
                Window* window = Application::SharedApp()->RootWindow();
                Application::SharedApp()->OnNativeHandleChanged(window);
                window->Show();
            }

            void JNICALL XliJ_OnTick (JNIEnv* env, jobject obj)
            {
                Xli::Application::SharedApp()->OnUpdateFrame();
            }            
        }

        static void AttachNativeCallbacks(jclass shim_class, JNIEnv* l_env)
        {
            LOGD("Registering native functions");
            static JNINativeMethod native_funcs[] = {
                {(char* const)"XliJ_JavaThrowError", (char* const)"(ILjava/lang/String;)V", (void *)&XliJ_JavaThrowError},
                {(char* const)"XliJ_UnoSurfaceReady", (char* const)"(Landroid/view/Surface;)V", (void *)&XliJ_UnoSurfaceReady},
                {(char* const)"XliJ_OnTick", (char* const)"(Landroid/view/Surface;)V", (void *)&XliJ_UnoSurfaceReady},
            };
            // the last argument is the number of native functions
            jint attached = l_env->RegisterNatives(shim_class, native_funcs, 2);
            if (attached < 0) {
                LOGE("COULD NOT REGISTER NATIVE FUNCTIONS");
                XLI_THROW("COULD NOT REGISTER NATIVE FUNCTIONS");
            } else {
                LOGD("Native functions registered");
            }
        }

        void Android::OnJNILoad(JNIEnv* env, jclass shim_class)
        {
            AttachNativeCallbacks(shim_class, env);
        }

        // Application* Android::SharedApp()
        // {
        //     return application;
        // }
        
        void Android::Init(struct android_app* native_app)
        {
            native_app->userData = 0;
            native_app->onAppCmd = handle_cmd;
            native_app->onInputEvent = handle_input;

            Xli::PlatformSpecific::AndroidApplication = native_app;
            AndroidActivity = native_app->activity;

            Out->SetStream(ManagePtr(new ALogStream(ANDROID_LOG_INFO)));
            Error->SetStream(ManagePtr(new ALogStream(ANDROID_LOG_WARN)));

            // Request VSync Callbacks
            // Android::RequestChoreographer();
        }
    }
}
