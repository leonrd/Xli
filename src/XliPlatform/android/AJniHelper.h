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

#ifndef __XLI_AJNI_ANDROID_H__
#define __XLI_AJNI_ANDROID_H__

#include <jni.h>
#include "ACrossThread.h"
#include "AShim.h"

namespace Xli
{
    namespace PlatformSpecific
    {
        class AJniHelper
        {
            JNIEnv* env;
            static JavaVM* vm;
            static jclass shim;
            static jobject activity;
            static jobject jAssetManager;
            static AAssetManager* assetManager;

            static void SetShim(JNIEnv* env, jclass globalRefdShim);
        public:
            static String ExternalDataPath;
            static String InternalDataPath;

            static void Init(JavaVM* jvm, JNIEnv* env, jclass globalRefdShim, jobject optionalActivity);
            AJniHelper();

            JNIEnv* GetEnv();
            JNIEnv* operator->();

            jobject GetActivity();
            AAssetManager* GetAssetManager();

            static jclass GetShim();
            static JavaVM* GetVM();

            // to be remove on unocore update
            static jmethodID GetInstanceMethod(JNIEnv* env, const char* m_name, const char* m_sig);
            static jmethodID GetInstanceMethod(JNIEnv* env, jobject inst, const char* m_name, const char* m_sig);
            static jobject GetInstance(JNIEnv* env, const char* class_name, const char* constructor_sig, ...);
            static jobject GetInstance(JNIEnv* env, jclass cls, const char* constructor_sig, ...);
            static int PrepareAssetJar(JNIEnv* env, const char* file_name, const char* class_name);
            static int PrepareAssetJar(JNIEnv* env, const char* file_name, const char* class_name, int package);
            static jclass GetAssetClass(JNIEnv* env, const char* file_name, const char* class_name);
        };
    };
};

#endif
