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
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
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
        static pthread_key_t JniThreadKey;

        //--------------------------------------------------
        // remove on unocore upgrade
        static unsigned char shimApkData[] =
        {
#include "shim/apk.inc"
        };
        void CTTextAction::Execute()
        {
            if (GlobalWindow) {
                GlobalEventHandler->OnTextInput(GlobalWindow, this->Text);
            }
        }
        void CTError::Execute()
        {
            String finalMessage = "XLiError (" + String(errorCode)+ ") - " + message;
            XLI_THROW(finalMessage.Ptr());
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
        void CTKeyboardResize::Execute()
        {
            if (GlobalWindow) {
                GlobalEventHandler->OnKeyboardResized(GlobalWindow);
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
            void JNICALL XliJ_OnKeyboardResized (JNIEnv *env , jobject obj)
            {
                CTQueue::EnqueueCrossThreadEvent(new CTKeyboardResize());
            }
        }
         static void AttachNativeCallbacks(jclass shimClass, JNIEnv* env)
         {
             LOGD("Registering native functions");
             static JNINativeMethod native_funcs[] = {
                 {(char* const)"XliJ_OnKeyUp", (char* const)"(I)V", (void *)&XliJ_OnKeyUp},
                 {(char* const)"XliJ_OnKeyDown", (char* const)"(I)V", (void *)&XliJ_OnKeyDown},
                 {(char* const)"XliJ_OnTextInput", (char* const)"(Ljava/lang/String;)V", (void *)&XliJ_OnTextInput},
                 {(char* const)"XliJ_JavaThrowError", (char* const)"(ILjava/lang/String;)V", (void *)&XliJ_JavaThrowError},
                 {(char* const)"XliJ_OnKeyboardResized", (char* const)"()V", (void *)&XliJ_OnKeyboardResized},
             };
// the last argument is the number of native functions
             jint attached = env->RegisterNatives(shimClass, native_funcs, 5);
             if (attached < 0) {
                 LOGE("COULD NOT REGISTER NATIVE FUNCTIONS");
                 XLI_THROW("COULD NOT REGISTER NATIVE FUNCTIONS");
             } else {
                 LOGD("Native functions registered");
             }
         }
        //--------------------------------------------------

        static void JniDestroyThread(void* value)
        {
            LOGD("JNI: Detaching current thread");

            JNIEnv* env = (JNIEnv*)value;
            AJniHelper::GetVM()->DetachCurrentThread();
            pthread_setspecific(JniThreadKey, NULL);
        }

        JavaVM* AJniHelper::vm;
        jclass AJniHelper::shim;
        jobject AJniHelper::activity;
        jobject AJniHelper::jAssetManager;
        AAssetManager* AJniHelper::assetManager;
        String AJniHelper::ExternalDataPath;
        String AJniHelper::InternalDataPath;
        void AJniHelper::Init(JavaVM* jvm, JNIEnv* env, jclass globalRefdShim, jobject optionalActivity)
        {
            vm = jvm;
            if (pthread_key_create(&JniThreadKey, JniDestroyThread))
                LOGE("JNI ERROR: Unable to create pthread key"); // Not fatal
            jAssetManager = 0;
            assetManager = 0;
            activity = 0;

            if (!globalRefdShim)
            {
                //--------------------------------------------------
                // REMOVE THIS PART ON UNOCORE UPDATE
                activity = optionalActivity;
                JNIEnv* ENVIR;
                if (vm->GetEnv(reinterpret_cast<void**>(&ENVIR), JNI_VERSION_1_6) != JNI_OK)
                {
                    if (vm->AttachCurrentThread(&ENVIR, NULL) != JNI_OK)
                        XLI_THROW("JNI ERROR: Failed to attach current thread");
                }
                LOGD("--Loading shim apk");
                PrepareAssetJar(ENVIR, "XliShimJ.apk","XliJ");
                globalRefdShim = GetAssetClass(ENVIR, "XliShimJ.apk","XliJ");
                SetShim(ENVIR, globalRefdShim);
                bool err = ENVIR->ExceptionCheck();
                jmethodID cacheAct = ENVIR->GetStaticMethodID(shim, "CacheActivity", "(Landroid/app/NativeActivity;)V");
                err = ENVIR->ExceptionCheck();
                ENVIR->CallStaticVoidMethod(shim, cacheAct, activity);
                AttachNativeCallbacks(shim, ENVIR);
                LOGD("--Done loading");
                //--------------------------------------------------
            } else {
                SetShim(env, globalRefdShim);
            }
        }
        AJniHelper::AJniHelper()
        {
            if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
            {
                if (vm->AttachCurrentThread(&env, NULL) != JNI_OK)
                    XLI_THROW("JNI ERROR: Failed to attach current thread");
                pthread_setspecific(JniThreadKey, (void*)env);
            }
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

        void AJniHelper::SetShim(JNIEnv* env, jclass globalRefdShim)
        {
            shim = globalRefdShim;
            AShim::CacheMids(env, globalRefdShim);
            // paths
            ExternalDataPath = "";
            InternalDataPath = "";
        }
        jclass AJniHelper::GetShim()
        {
            return shim;
        }
        JavaVM* AJniHelper::GetVM()
        {
            return vm;
        }
        jobject AJniHelper::GetActivity()
        {
            //activity
            if (!activity)
            {
                jmethodID getActivity = env->GetStaticMethodID(shim, "GetActivity", "()Landroid/app/Activity;");
                activity = reinterpret_cast<jclass>(env->NewGlobalRef(env->CallStaticObjectMethod(shim, getActivity)));
                if (activity==0) { XLI_THROW("JNI ERROR: Failed to grab activity object"); }
            }
            return activity;
        }
        AAssetManager* AJniHelper::GetAssetManager()
        {
            if (!assetManager)
            {
                // assetManager
                jmethodID getAssetManager = env->GetStaticMethodID(shim, "GetAssetManager", "()Landroid/content/res/AssetManager;");
                jobject jAssetManager = env->NewGlobalRef(env->CallStaticObjectMethod(shim, getAssetManager));
                assetManager = AAssetManager_fromJava(env, jAssetManager);
                if (assetManager==0) { XLI_THROW("JNI ERROR: Failed to grab assetManager object"); }
            }
            return assetManager;
        }

        //--------------------------------------------------
        //to be removed on unocore update
        int AJniHelper::PrepareAssetJar(JNIEnv* env, const char* file_name, const char* class_name)
        {
            return PrepareAssetJar(env, file_name, class_name, 1);
        }
        jmethodID AJniHelper::GetInstanceMethod(JNIEnv* env, const char* m_name, const char* m_sig)
        {
            return GetInstanceMethod(env, activity, m_name, m_sig);
        }
        jmethodID AJniHelper::GetInstanceMethod(JNIEnv* env, jobject inst, const char* m_name, const char* m_sig)
        {
            return env->GetMethodID(env->GetObjectClass(inst), m_name, m_sig);
        }
        jobject AJniHelper::GetInstance(JNIEnv* env, const char* class_name, const char* constructor_sig, ...)
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
        jobject AJniHelper::GetInstance(JNIEnv* env, jclass cls, const char* constructor_sig, ...)
        {
            jmethodID mid = env->GetMethodID(cls, "<init>", constructor_sig);
            jobject result = 0;
            va_list args;
            va_start(args, constructor_sig);
            result = env->NewObjectV(cls, mid, args);
            va_end(args);
            return result;
        }

        int AJniHelper::PrepareAssetJar(JNIEnv* env, const char* file_name, const char* class_name, int package)
        {
            jmethodID get_dir = GetInstanceMethod(env, "getDir","(Ljava/lang/String;I)Ljava/io/File;");
            if (!get_dir) { LOGE("Could not find get_dir"); return 0; }
//dex-file
            jobject dex_dir_file = env->CallObjectMethod(activity, get_dir, env->NewStringUTF("dex"), (jint)0);
            if (!dex_dir_file) { LOGE("Could not find dex_dir_file"); return 0; }
//Find destination file
            jobject dex_file = GetInstance(env, "java/io/File","(Ljava/io/File;Ljava/lang/String;)V", dex_dir_file, env->NewStringUTF(file_name));
            if (!dex_file) { LOGE("Could not find dex_file"); return 0; }
//get destination path
            jmethodID getAbsPath = env->GetMethodID(env->FindClass("java/io/File"), "getAbsolutePath", "()Ljava/lang/String;");
            jstring dex_internal_path = (jstring)env->CallObjectMethod(dex_file, getAbsPath);
            const char *filepath = env->GetStringUTFChars(dex_internal_path, 0);
//write apk
            FILE* appConfigFile = fopen(filepath, "w+");
            int file_len = sizeof(shimApkData);
            LOGI("App config file created successfully. Writing config data ...\n");
            int32_t res = fwrite((void*)shimApkData, sizeof(char), file_len, appConfigFile);
            if (file_len != res)
            {
                LOGE("Error generating apk.\n");
                return 0;
            }
            fclose(appConfigFile);
            env->ReleaseStringUTFChars(dex_internal_path, filepath);
            return 1;
        }
        jclass AJniHelper::GetAssetClass(JNIEnv* env, const char* file_name, const char* class_name)
        {
            jmethodID get_dir = GetInstanceMethod(env, "getDir","(Ljava/lang/String;I)Ljava/io/File;");
            jobject dex_dir_file = env->CallObjectMethod(activity, get_dir, env->NewStringUTF("dex"), (jint)0);
            jclass file_cls = env->FindClass("java/io/File");
            jmethodID getAbsPath = env->GetMethodID(file_cls, "getAbsolutePath", "()Ljava/lang/String;");
            jclass dexloader_cls = env->FindClass("dalvik/system/DexClassLoader");
            jmethodID load_class = env->GetMethodID(dexloader_cls, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
            jmethodID get_classloader = GetInstanceMethod(env, "getClassLoader","()Ljava/lang/ClassLoader;");
            jobject classloader = env->CallObjectMethod(activity, get_classloader);
            jobject dex_file = GetInstance(env, "java/io/File","(Ljava/io/File;Ljava/lang/String;)V", dex_dir_file, env->NewStringUTF(file_name));
            jobject outdex_dir_file = env->CallObjectMethod(activity, get_dir, env->NewStringUTF("outdex"), (jint)0);
            jstring dex_optimized_out_path = (jstring)env->CallObjectMethod(outdex_dir_file, getAbsPath);
            jstring dex_internal_path = (jstring)env->CallObjectMethod(dex_file, getAbsPath);
            jobject dexloader = GetInstance(env, "dalvik/system/DexClassLoader", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)V", dex_internal_path, dex_optimized_out_path, NULL, classloader);
            jclass loaded = (jclass)env->CallObjectMethod(dexloader, load_class, env->NewStringUTF(class_name));
            jclass globalClass = reinterpret_cast<jclass>(env->NewGlobalRef(loaded));
            return globalClass;
        }
    }
}
