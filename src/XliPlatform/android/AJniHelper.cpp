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
#include <XliPlatform/Window.h>
#include <XliHttpClient.h>
#include <pthread.h>
#include <stdarg.h>
#include "ACrossThread.h"
#include "AJniHelper.h"

#define DEBUG_JNI

extern Xli::WindowEventHandler* GlobalEventHandler;
extern Xli::Window* GlobalWindow;

namespace Xli
{
    namespace PlatformSpecific
    {
        void CTTextAction::Execute() 
        {
            if (GlobalWindow) {
                GlobalEventHandler->OnTextInput(GlobalWindow, this->Text);
            }
        }
        void CTKeyAction::Execute() 
        {
            if (GlobalWindow) {
                if (this->KeyDown)
                {
                    GlobalEventHandler->OnKeyDown(GlobalWindow, this->KeyEvent);
                } else {
                    GlobalEventHandler->OnKeyUp(GlobalWindow, this->KeyEvent); 
                }
            }
        }

        extern "C"
        {
            void JNICALL XliJ_OnKeyUp (JNIEnv *env , jobject obj, jint keyCode) 
            {
                CTQueue::EnqueueCrossThreadEvent(new CTKeyAction((AKeyEvent)keyCode, false));
            }

            void JNICALL XliJ_OnKeyDown (JNIEnv *env , jobject obj, jint keyCode) 
            {
                CTQueue::EnqueueCrossThreadEvent(new CTKeyAction((AKeyEvent)keyCode, true));
            }

            void JNICALL XliJ_OnTextInput (JNIEnv *env , jobject obj, jstring keyChars) 
            {
                const char* jChars = env->GetStringUTFChars((jstring)keyChars, NULL);        
                CTQueue::EnqueueCrossThreadEvent(new CTTextAction(String(jChars)));
                env->ReleaseStringUTFChars((jstring)keyChars, jChars);
            }

            void JNICALL XliJ_JavaThrowError (JNIEnv *env , jobject obj, jint errorCode, jstring errorMessage) 
            {
                char const* cerrorMessage = env->GetStringUTFChars(errorMessage, NULL);
                String finalMessage = String("JavaThrown:")+String(cerrorMessage); 
                CTQueue::EnqueueCrossThreadEvent(new CTError(finalMessage, errorCode));
                env->ReleaseStringUTFChars(errorMessage, cerrorMessage);
            }
        }

        struct ANativeActivity* AndroidActivity = 0;
        
        static pthread_key_t JniThreadKey;
        static pthread_key_t JniShimKey;

        static void JniDestroyThread(void* value)
        {
            LOGD("JNI: Detaching current thread");

            JNIEnv* env = (JNIEnv*)value;
            AndroidActivity->vm->DetachCurrentThread();
            pthread_setspecific(JniThreadKey, NULL);
        }

        static void JniDestroyShim(void* value)
        {
            // where can we DeleteGlobalRef(*shim) ?
            LOGD("JNI: Freeing Shim class");
            jclass* shim = (jclass*)value;
            delete shim;
            pthread_setspecific(JniShimKey, NULL);
        }

        static void AttachNativeCallbacks(jclass* shim_class, JNIEnv *l_env)
        {
            LOGD("Registering native functions");
            static JNINativeMethod native_funcs[] = {
                {(char* const)"XliJ_OnKeyUp", (char* const)"(I)V", (void *)&XliJ_OnKeyUp},
                {(char* const)"XliJ_OnKeyDown", (char* const)"(I)V", (void *)&XliJ_OnKeyDown},
                {(char* const)"XliJ_OnTextInput", (char* const)"(Ljava/lang/String;)V", (void *)&XliJ_OnTextInput},
                {(char* const)"XliJ_JavaThrowError", (char* const)"(ILjava/lang/String;)V", (void *)&XliJ_JavaThrowError},
            };
            // the last argument is the number of native functions
            jint attached = l_env->RegisterNatives(*shim_class, native_funcs, 4);
            if (attached < 0) {
                LOGE("COULD NOT REGISTER NATIVE FUNCTIONS");
                XLI_THROW("COULD NOT REGISTER NATIVE FUNCTIONS");
            } else {
                LOGD("Native functions registered");
            }
        }

        static void CacheNativeActivity(jclass* shim_class, JNIEnv* env, jobject activity) 
        {
            jmethodID mid = env->GetStaticMethodID(*shim_class, "CacheActivity", "(Landroid/app/NativeActivity;)V");
            env->CallStaticVoidMethod(*shim_class, mid, activity);
        }


        void AJniHelper::Init()
        {
            if (pthread_key_create(&JniThreadKey, JniDestroyThread))
                LOGE("JNI ERROR: Unable to create pthread key"); // Not fatal

            if (pthread_key_create(&JniShimKey, JniDestroyShim))
                LOGE("JNI ERROR: Unable to create shim pthread key"); // Not fatal
        }

        AJniHelper::AJniHelper()
        {
            if (AndroidActivity->vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
            {
                if (AndroidActivity->vm->AttachCurrentThread(&env, NULL) != JNI_OK)
                    XLI_THROW("JNI ERROR: Failed to attach current thread");

                jclass tmpCls = GetCustomClass("com/Shim/XliJ");
                jclass *shim_class = new jclass;
                *shim_class = reinterpret_cast<jclass>(env->NewGlobalRef(tmpCls));

                pthread_setspecific(JniThreadKey, (void*)env);
                pthread_setspecific(JniShimKey, (void*)shim_class);
                AShim::CacheMids(env, *shim_class);
                CacheNativeActivity(shim_class, env, AndroidActivity->clazz);
                AttachNativeCallbacks(shim_class, env);
            }

            jclass* shim_p = reinterpret_cast<jclass*>(pthread_getspecific(JniShimKey));

            if (shim_p)
            {
                shim = *shim_p;
            } 
            else 
            {
                LOGE("could not get shim");
            }
        }

        jclass AJniHelper::GetShim()
        {
            return shim;
        }

        jmethodID AJniHelper::FindMethod(const char* className, const char* methodName, const char* methodSig)
        {
            jclass cls = env->FindClass(className);
            if (!cls) 
                XLI_THROW((String)"Failed to get JNI class '" + className + "'");

            jmethodID method = env->GetMethodID(cls, methodName, methodSig);
            if (!method) 
                XLI_THROW((String)"Failed to get JNI method '" + className + "." + methodName + methodSig + "'");

            return method;
        }

        jobject AJniHelper::CallObjectMethod(jobject inst, const char* name, const char* sig)
        {
            jclass cls = env->GetObjectClass(inst);

#ifdef DEBUG_JNI
            if (!cls) 
                XLI_THROW((String)"Failed to get JNI class for method '" + name + "'");
#endif

            jmethodID method = env->GetMethodID(cls, name, sig);

#ifdef DEBUG_JNI
            if (!method) 
                XLI_THROW((String)"Failed to get JNI method '" + name + "'");
#endif

            return env->CallObjectMethod(inst, method);
        }

        String AJniHelper::GetString(jobject str)
        {
            const char* utf8 = env->GetStringUTFChars((jstring)str, NULL);
            String result = utf8;
            env->ReleaseStringUTFChars((jstring)str, utf8);
            return result;
        }

        JNIEnv* AJniHelper::GetEnv()
        {
            // TODO: Check jni exceptions
            return env;
        }

        JNIEnv* AJniHelper::operator->()
        {
            // TODO: Check jni exceptions
            return env;
        }
        
        jmethodID AJniHelper::GetInstanceMethod(const char* m_name, const char* m_sig)
        {
            return GetInstanceMethod(GetInstance(), m_name, m_sig);
        }

        jmethodID AJniHelper::GetInstanceMethod(jobject inst, const char* m_name, const char* m_sig)
        {
            return env->GetMethodID(env->GetObjectClass(inst), m_name, m_sig);
        }

        jobject AJniHelper::GetInstance() 
        {
            return AndroidActivity->clazz;
        }        

        jobject AJniHelper::GetInstance(const char* class_name, const char* constructor_sig, ...)
        {
            jclass cls = env->FindClass(class_name);
            if (cls) {
                jmethodID mid = env->GetMethodID(cls, "<init>", constructor_sig);
                if (mid) {
                    jobject result = 0;
                    va_list args;
                    va_start(args, constructor_sig);
                    result = env->NewObjectV(cls, mid, args);
                    va_end(args);
                    return result;
                } else {
                    LOGE( "GetInstance Could not get <init> for class");
                    return NULL;
                }
            } else {
                LOGE( "GetInstance Could not find class");
                return NULL;
            }
        }

        jobject AJniHelper::GetInstance(jclass cls, const char* constructor_sig, ...)
        {
            jmethodID mid = env->GetMethodID(cls, "<init>", constructor_sig);
            jobject result = 0;
            va_list args;
            va_start(args, constructor_sig);
            result = env->NewObjectV(cls, mid, args);
            va_end(args);
            return result;
        }

        jclass AJniHelper::GetCustomClass(const char* class_path)
        {
            jclass activityClass = env->FindClass("android/app/NativeActivity");
            jmethodID getClassLoader = env->GetMethodID(activityClass,"getClassLoader", "()Ljava/lang/ClassLoader;");
            jobject cls = env->CallObjectMethod(AndroidActivity->clazz, getClassLoader);
            jclass classLoader = env->FindClass("java/lang/ClassLoader");
            jmethodID findClass = env->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
            jstring strClassName = env->NewStringUTF(class_path);
            jclass classIWant = (jclass)env->CallObjectMethod(cls, findClass, strClassName);
        }
    }
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    LOGE ("\n\n&&&&&&&&&&&&& BOOOYA MOTHERFUCKER &&&&&&&&&&&&&&&\n\n");

    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        LOGE ("&&&&&&& GetEnv failed &&&&&&");
        return -1;
    }

    jclass shimClass = env->FindClass("com/Shim/XliJ");

    LOGE ("\n\n&&&&&&&&&&&&& BOOOYA FOTHERMUCKER &&&&&&&&&&&&&&&\n\n");
    
    return JNI_VERSION_1_6;
}

