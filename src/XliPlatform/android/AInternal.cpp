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

// {TODO} File references Application::SharedApp() very frequently, probably best to
//        cache this like we used to in the old AWindow

static Xli::MutexQueue<Xli::PlatformSpecific::CTAction*> cross_thread_event_queue;

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
                //
                String m = "XLiError (" + String(errorCode)+ ") - " + finalMessage;
                XLI_THROW(m.Ptr());
                //cross_thread_event_queue.Enqueue(new CTError(finalMessage, errorCode));
                env->ReleaseStringUTFChars(errorMessage, cerrorMessage);
            }

            void JNICALL XliJ_UnoSurfaceReady (JNIEnv* env, jobject obj, jobject unoSurface)
            {
                //
                Window* window = Application::SharedApp()->RootWindow();
                Application::SharedApp()->OnNativeHandleChanged(window);
                window->Show();
                //cross_thread_event_queue.Enqueue(new CTSurfaceReady());
            }

            void JNICALL XliJ_SurfaceSizeChanged (JNIEnv* env, jobject obj, int width, int height)
            {
                //
                Application* app = Xli::Application::SharedApp();
                app->OnSizeChanged(app->RootWindow());
                //cross_thread_event_queue.Enqueue(new CTSurfaceSizeChanged());
            }

            void JNICALL XliJ_OnSurfaceTouch(JNIEnv* env, jobject obj, int pointerID, int x, int y, int type)
            {
                //
                Application* app = Xli::Application::SharedApp();
                Window* win = app->RootWindow();
                switch (type)
                {
                case 0:
                    app->OnTouchMove(win, Vector2(x, y), pointerID);
                    break;
                case 1:
                    app->OnTouchDown(win, Vector2(x, y), pointerID);
                    break;
                case 2:
                    app->OnTouchUp(win, Vector2(x, y), pointerID);
                    break;
                }
                //cross_thread_event_queue.Enqueue(new CTTouchEvent(pointerID, x, y, type));
            }

            void JNICALL XliJ_TimerCallback (JNIEnv* env, jobject obj, int timerID)
            {

            }

            void JNICALL XliJ_OnKeyboardResized (JNIEnv* env, jobject obj)
            {
                //
                Application* app = Xli::Application::SharedApp();
                Window* win = app->RootWindow();
                app->OnKeyboardResized(win);
                //cross_thread_event_queue.Enqueue(new CTKeyboardResize());
            }

            void JNICALL XliJ_OnKeyUp (JNIEnv *env , jobject obj, jint keyCode)
            {
                //
                Application* app = Xli::Application::SharedApp();
                Window* win = app->RootWindow();
                app->OnKeyUp(win, AndroidToXliKeyEvent((AKeyEvent)keyCode));
                //cross_thread_event_queue.Enqueue(new CTKeyAction((AKeyEvent)keyCode, false));
            }
            void JNICALL XliJ_OnKeyDown (JNIEnv *env , jobject obj, jint keyCode)
            {
                //
                Application* app = Xli::Application::SharedApp();
                Window* win = app->RootWindow();
                app->OnKeyDown(win, AndroidToXliKeyEvent((AKeyEvent)keyCode));
                //cross_thread_event_queue.Enqueue(new CTKeyAction((AKeyEvent)keyCode, true));
            }
            void JNICALL XliJ_OnTextInput (JNIEnv *env , jobject obj, jstring keyChars)
            {
                const char* jChars = env->GetStringUTFChars((jstring)keyChars, NULL);
                //
                Application* app = Xli::Application::SharedApp();
                Window* win = app->RootWindow();
                app->OnTextInput(win, String(jChars));
                //cross_thread_event_queue.Enqueue(new CTTextAction(String(jChars)));
                env->ReleaseStringUTFChars((jstring)keyChars, jChars);
            }

            // void JNICALL cppOnCreate(JNIEnv *env , jobject obj) see XliMain :)
            // {
            // }
            void JNICALL cppOnDestroy(JNIEnv *env , jobject obj)
            {
                Application::SharedApp()->Terminate();
            }
            void JNICALL cppOnPause(JNIEnv *env , jobject obj)
            {
                Application::SharedApp()->ResignActive();
            }
            void JNICALL cppOnResume(JNIEnv *env , jobject obj)
            {
            }
            void JNICALL cppOnStart(JNIEnv *env , jobject obj)
            {
            }
            void JNICALL cppOnStop(JNIEnv *env , jobject obj)
            {
                Application::SharedApp()->EnterBackground();
            }
            void JNICALL cppOnSaveInstanceState(JNIEnv *env , jobject obj, jobject state)
            {
            }
            void JNICALL cppOnConfigChanged(JNIEnv *env , jobject obj)
            {
            }
            void JNICALL cppOnLowMemory(JNIEnv *env , jobject obj)
            {
                Application::SharedApp()->OnLowMemory();
            }
            void JNICALL cppOnWindowFocusChanged(JNIEnv *env , jobject obj, bool hasFocus)
            {
                if (hasFocus)
                {
                    Application::SharedApp()->BecomeActive();
                } else {
                    Application::SharedApp()->ResignActive();
                }
            }
            void JNICALL cppOnVSync (JNIEnv* env, jobject obj, int milliseconds)
            {
                Window* window = Application::SharedApp()->RootWindow();
                //Android::ProcessCrossThreadEvents(); // removed as all events in correct thread
                if (window->CurrentState() == Window::Visible)
                {
                    window->GetContext()->MakeCurrent(window);
                    Application::SharedApp()->OnUpdateFrame();
                }
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

        static void AttachNativeCallbacks(JNIEnv* l_env, jclass shim_class)
        {
            LOGD("Registering native functions");
            static JNINativeMethod native_funcs[] = {
                {(char* const)"XliJ_JavaThrowError", (char* const)"(ILjava/lang/String;)V", (void *)&XliJ_JavaThrowError},
                {(char* const)"XliJ_UnoSurfaceReady", (char* const)"(Landroid/view/Surface;)V", (void *)&XliJ_UnoSurfaceReady},
                {(char* const)"XliJ_SurfaceSizeChanged", (char* const)"(II)V", (void *)&XliJ_SurfaceSizeChanged},
                {(char* const)"XliJ_OnSurfaceTouch", (char* const)"(IIII)V", (void *)&XliJ_OnSurfaceTouch},
                {(char* const)"XliJ_TimerCallback", (char* const)"(I)V", (void *)&XliJ_TimerCallback},
                {(char* const)"XliJ_OnKeyboardResized", (char* const)"()V", (void *)&XliJ_OnKeyboardResized},
                {(char* const)"XliJ_OnKeyUp", (char* const)"(I)V", (void *)&XliJ_OnKeyUp},
                {(char* const)"XliJ_OnKeyDown", (char* const)"(I)V", (void *)&XliJ_OnKeyDown},
                {(char* const)"XliJ_OnTextInput", (char* const)"(Ljava/lang/String;)V", (void *)&XliJ_OnTextInput},

                {(char* const)"cppOnDestroy", (char* const)"()V", (void *)&cppOnDestroy},
                {(char* const)"cppOnPause", (char* const)"()V", (void *)&cppOnPause},
                {(char* const)"cppOnResume", (char* const)"()V", (void *)&cppOnResume},
                {(char* const)"cppOnStart", (char* const)"()V", (void *)&cppOnStart},
                {(char* const)"cppOnStop", (char* const)"()V", (void *)&cppOnStop},
                {(char* const)"cppOnSaveInstanceState", (char* const)"(Landroid/os/Bundle;)V", (void *)&cppOnSaveInstanceState},
                {(char* const)"cppOnConfigChanged", (char* const)"()V", (void *)&cppOnConfigChanged},
                {(char* const)"cppOnLowMemory", (char* const)"()V", (void *)&cppOnLowMemory},
                {(char* const)"cppOnWindowFocusChanged", (char* const)"(Z)V", (void *)&cppOnWindowFocusChanged},
                {(char* const)"cppOnVSync", (char* const)"(I)V", (void *)&cppOnVSync},
            };
            // the last argument is the number of native functions
            jint attached = l_env->RegisterNatives(shim_class, native_funcs, 19);
            if (attached < 0) {
                LOGE("COULD NOT REGISTER NATIVE FUNCTIONS");
                XLI_THROW("COULD NOT REGISTER NATIVE FUNCTIONS");
            } else {
                LOGD("Native functions registered");
            }
        }

        void Android::OnJNILoad(JavaVM* vm, JNIEnv* env, jclass shim_class)
        {
            Xli::PlatformSpecific::AJniHelper::Init(vm, env, shim_class);
            AttachNativeCallbacks(env, shim_class);
        }

        void Android::Init()
        {
            Out->SetStream(ManagePtr(new ALogStream(ANDROID_LOG_INFO)));
            Error->SetStream(ManagePtr(new ALogStream(ANDROID_LOG_WARN)));
        }
    }
}
