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

        JavaVM* AJniHelper::vm;
        jclass AJniHelper::shim;
        jobject AJniHelper::activity;
        jobject AJniHelper::jAssetManager;
        AAssetManager* AJniHelper::assetManager;
        String AJniHelper::ExternalDataPath;
        String AJniHelper::InternalDataPath;

        static void JniDestroyThread(void* value)
        {
            LOGD("JNI: Detaching current thread");

            JNIEnv* env = (JNIEnv*)value;
            AJniHelper::GetVM()->DetachCurrentThread();
            pthread_setspecific(JniThreadKey, NULL);
        }

        void AJniHelper::Init(JavaVM* jvm, JNIEnv* env, jclass globalRefdShim)
        {
            vm = jvm;
            if (pthread_key_create(&JniThreadKey, JniDestroyThread))
                LOGE("JNI ERROR: Unable to create pthread key"); // Not fatal
            SetShim(env, globalRefdShim);
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
            // assetManager
            jmethodID getAssetManager = env->GetStaticMethodID(shim, "GetAssetManager", "()Landroid/content/res/AssetManager;");
            jobject jAssetManager = env->NewGlobalRef(env->CallStaticObjectMethod(shim, getAssetManager));
            assetManager = AAssetManager_fromJava(env, jAssetManager);
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
            return assetManager;
        }
    }
}
