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

#include <Xli/MutexQueue.h>
#include <Xli/Console.h>
#include <XliGL/GLContext.h>

#include "../../../3rdparty/android_native_app_glue/android_native_app_glue.h"
#include "AInternal.h"
#include "AShim.h"
#include "AKeyEvent.h"
#include "ALogStream.h"

static Xli::MutexQueue<Xli::PlatformSpecific::CTAction*> cross_thread_event_queue;
//static Xli::Application* application;

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
                Application::SharedApp()->ResignActive();
                break;
            case APP_CMD_STOP:
                Application::SharedApp()->EnterBackground();
                break;
            case (APP_CMD_GAINED_FOCUS):
                Application::SharedApp()->BecomeActive();
                break;
            case (APP_CMD_LOST_FOCUS):
                Application::SharedApp()->ResignActive();
                break;
            case APP_CMD_DESTROY:
                Application::SharedApp()->Terminate();
                break;
            case APP_CMD_LOW_MEMORY:
                Application::SharedApp()->OnLowMemory();
                break;
            }
        }

        static int32_t handle_input(struct android_app* native_app, AInputEvent* event)
        {
            // We want to trap backbutton & menubutton events but let the rest go to android
            // we will be capturing input and text events from java objects
            Application* app = Application::SharedApp();
            Window* win = app->RootWindow();
            if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
            {
                if (AKeyEvent_getKeyCode(event) == AKEYCODE_MENU)
                {
                    int32_t action = AKeyEvent_getAction(event);
                    if (action == AKEY_EVENT_ACTION_DOWN) {
                        return app->OnKeyDown(win, KeyMenu);
                    } else if (action == AKEY_EVENT_ACTION_UP) {
                        return app->OnKeyUp(win, KeyMenu);
                    }
                } else if (AKeyEvent_getKeyCode(event) == AKEYCODE_BACK) {
                    int32_t action = AKeyEvent_getAction(event);
                    if (action == AKEY_EVENT_ACTION_UP) {
                       // we only release on 'up' and minimize keyboard doesnt give
                       // us a down event. This way we get a consistent behaviour
                       // at the expense of a little detail. This may have to
                       // be reviewed but feels good for now
                        bool r1 = app->OnKeyDown(win, BackButton);
                        bool r2 = app->OnKeyUp(win, BackButton);
                        //return (r1 || r2); // {TODO} this is the correct method, however until we 
                        //                             make this safe for our nativeactivity it is 
                        //                             just safer to eat the event
                        return true;
                    }
                }
            }
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
                cross_thread_event_queue.Enqueue(new CTSurfaceReady());
            }

            void JNICALL XliJ_SurfaceSizeChanged (JNIEnv* env, jobject obj, int width, int height)
            {
                cross_thread_event_queue.Enqueue(new CTSurfaceSizeChanged());
            }

            void JNICALL XliJ_OnSurfaceTouch(JNIEnv* env, jobject obj, int pointerID, int x, int y, int type)
            {
                cross_thread_event_queue.Enqueue(new CTTouchEvent(pointerID, x, y, type));
            }

            void JNICALL XliJ_FrameTick (JNIEnv* env, jobject obj, int milliseconds)
            {
                Window* window = Application::SharedApp()->RootWindow();
                PlatformSpecific::Android::ProcessMessages();
                PlatformSpecific::Android::ProcessCrossThreadEvents();
                if (window->CurrentState() == Window::Visible)
                {
                    Application::SharedApp()->OnUpdateFrame();
                }
            }

            void JNICALL XliJ_TimerCallback (JNIEnv* env, jobject obj, int timerID)
            {
                
            }

            void JNICALL XliJ_OnKeyboardResized (JNIEnv* env, jobject obj)
            {
                cross_thread_event_queue.Enqueue(new CTKeyboardResize());
            }

            void JNICALL XliJ_OnKeyUp (JNIEnv *env , jobject obj, jint keyCode)
            {
                cross_thread_event_queue.Enqueue(new CTKeyAction((AKeyEvent)keyCode, false));
            }
            void JNICALL XliJ_OnKeyDown (JNIEnv *env , jobject obj, jint keyCode)
            {
                cross_thread_event_queue.Enqueue(new CTKeyAction((AKeyEvent)keyCode, true));
            }
            void JNICALL XliJ_OnTextInput (JNIEnv *env , jobject obj, jstring keyChars)
            {
                const char* jChars = env->GetStringUTFChars((jstring)keyChars, NULL);
                cross_thread_event_queue.Enqueue(new CTTextAction(String(jChars)));
                env->ReleaseStringUTFChars((jstring)keyChars, jChars);
            }
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

        static void AttachNativeCallbacks(jclass shim_class, JNIEnv* l_env)
        {
            LOGD("Registering native functions");
            static JNINativeMethod native_funcs[] = {
                {(char* const)"XliJ_JavaThrowError", (char* const)"(ILjava/lang/String;)V", (void *)&XliJ_JavaThrowError},
                {(char* const)"XliJ_UnoSurfaceReady", (char* const)"(Landroid/view/Surface;)V", (void *)&XliJ_UnoSurfaceReady},
                {(char* const)"XliJ_SurfaceSizeChanged", (char* const)"(II)V", (void *)&XliJ_SurfaceSizeChanged},
                {(char* const)"XliJ_OnSurfaceTouch", (char* const)"(IIII)V", (void *)&XliJ_OnSurfaceTouch},
                {(char* const)"XliJ_FrameTick", (char* const)"(I)V", (void *)&XliJ_FrameTick},
                {(char* const)"XliJ_TimerCallback", (char* const)"(I)V", (void *)&XliJ_TimerCallback},
                {(char* const)"XliJ_OnKeyboardResized", (char* const)"()V", (void *)&XliJ_OnKeyboardResized},
                {(char* const)"XliJ_OnKeyUp", (char* const)"(I)V", (void *)&XliJ_OnKeyUp},
                {(char* const)"XliJ_OnKeyDown", (char* const)"(I)V", (void *)&XliJ_OnKeyDown},
                {(char* const)"XliJ_OnTextInput", (char* const)"(Ljava/lang/String;)V", (void *)&XliJ_OnTextInput},
            };
            // the last argument is the number of native functions
            jint attached = l_env->RegisterNatives(shim_class, native_funcs, 10);
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

        void Android::Init(struct android_app* native_app)
        {
            native_app->userData = 0;
            native_app->onAppCmd = handle_cmd;
            native_app->onInputEvent = handle_input;

            Xli::PlatformSpecific::AndroidApplication = native_app;
            AndroidActivity = native_app->activity;

            Out->SetStream(ManagePtr(new ALogStream(ANDROID_LOG_INFO)));
            Error->SetStream(ManagePtr(new ALogStream(ANDROID_LOG_WARN)));

            //application = Xli::Application::SharedApp();
        }
    }
}
