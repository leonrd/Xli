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
            jclass shim;
            static int shim_loaded;

        public:
            static void Init();

            AJniHelper();

            jclass GetShim();

            jmethodID FindMethod(const char* className, const char* methodName, const char* methodSig);

            jobject CallObjectMethod(jobject inst, const char* name, const char* sig);

            String GetString(jobject str);
            JNIEnv* GetEnv();

            JNIEnv* operator->();

            jmethodID GetInstanceMethod(const char* m_name, const char* m_sig);
            jmethodID GetInstanceMethod(jobject inst, const char* m_name, const char* m_sig);
            jobject GetInstance();
            jobject GetInstance(const char* class_name, const char* constructor_sig, ...);
            jobject GetInstance(jclass cls, const char* constructor_sig, ...);
            jclass GetCustomClass(const char* class_path);
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
