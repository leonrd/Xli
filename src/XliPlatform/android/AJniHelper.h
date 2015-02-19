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

            static void Init(JavaVM* jvm, JNIEnv* env, jclass globalRefdShim);
            AJniHelper();

            JNIEnv* GetEnv();
            JNIEnv* operator->();

            static jclass GetShim();
            static JavaVM* GetVM();
            static jobject GetActivity();
            static AAssetManager* GetAssetManager();
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
            virtual void Execute()
            {
                String finalMessage = "XLiError (" + String(errorCode)+ ") - " + message;
                XLI_THROW(finalMessage.Ptr());
            }
        };
    };
};

#endif
