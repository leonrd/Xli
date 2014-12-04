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

#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <XliPlatform/PlatformSpecific/Android.h>
#include "AShim.h"
#include "AJniHelper.h"
#include <XliPlatform/Window.h>

//{TODO} All these mids should be cacheable

namespace Xli
{
    namespace PlatformSpecific
    {
        int AShim::kbVisible = 0;

        jmethodID AShim::raiseKeyboard;
        jmethodID AShim::hideKeyboard;
        jmethodID AShim::getKeyboardSize;
        jmethodID AShim::showMessageBox;
        jmethodID AShim::connectedToNetwork;
        jmethodID AShim::httpShowHeaders;
        jmethodID AShim::initDefaultCookieManager;
        jmethodID AShim::getAssetManager;
        jmethodID AShim::hideStatusBar;
        jmethodID AShim::showStatusBar;
        jmethodID AShim::getStatusBarHeight;
        jmethodID AShim::getDisplayMetrics;
        jmethodID AShim::hasVibrator;
        jmethodID AShim::vibrateForMilliseconds;
        jmethodID AShim::abortAsyncTask;
        jmethodID AShim::holdObject;
        jmethodID AShim::getObject;
        jmethodID AShim::tryReleaseObject;
        jmethodID AShim::asyncInputStreamToString;
        jmethodID AShim::asyncInputStreamToByteArray;
        jmethodID AShim::getHeaderMap;
        jmethodID AShim::sendHttpAsyncA;
        jmethodID AShim::sendHttpAsyncB;

        void AShim::CacheMids(JNIEnv *env, jclass shimClass)
        {
            //LOGD("in_0");
            LOGD("Caching Mids");
            raiseKeyboard = env->GetStaticMethodID(shimClass, "RaiseKeyboard", "()V");
            hideKeyboard = env->GetStaticMethodID(shimClass, "HideKeyboard", "()V");
            getKeyboardSize = env->GetStaticMethodID(shimClass, "GetKeyboardSize", "()I");
            showMessageBox = env->GetStaticMethodID(shimClass, "ShowMessageBox", "(Ljava/lang/CharSequence;Ljava/lang/CharSequence;II)I");
            connectedToNetwork = env->GetStaticMethodID(shimClass, "ConnectedToNetwork", "()Z");
            initDefaultCookieManager = env->GetStaticMethodID(shimClass, "InitDefaultCookieManager", "()V");
            abortAsyncTask = env->GetStaticMethodID(shimClass, "AbortAsyncTask", "(I)V");
            getAssetManager = env->GetStaticMethodID(shimClass, "GetAssetManager", "()Landroid/content/res/AssetManager;");
            hideStatusBar = env->GetStaticMethodID(shimClass, "HideStatusBar", "()V");
            showStatusBar = env->GetStaticMethodID(shimClass, "ShowStatusBar", "()V");
            getStatusBarHeight = env->GetStaticMethodID(shimClass, "GetStatusBarHeight", "()F");
            getDisplayMetrics = env->GetStaticMethodID(shimClass, "GetDisplayMetrics", "()Landroid/util/DisplayMetrics;");
            holdObject = env->GetStaticMethodID(shimClass, "HoldObject", "(Ljava/lang/Object;)I");
            getObject = env->GetStaticMethodID(shimClass, "GetObject", "(I)Ljava/lang/Object;");
            tryReleaseObject = env->GetStaticMethodID(shimClass, "TryReleaseObject", "(I)Z");
            hasVibrator = env->GetStaticMethodID(shimClass, "HasVibrator", "()Z");
            vibrateForMilliseconds = env->GetStaticMethodID(shimClass, "VibrateForMilliseconds", "(I)V");
            asyncInputStreamToString = env->GetStaticMethodID(shimClass, "AsyncInputStreamToString", "(IJ)I");
            asyncInputStreamToByteArray = env->GetStaticMethodID(shimClass, "AsyncInputStreamToByteArray", "(IJ)I");
            getHeaderMap = env->GetStaticMethodID(shimClass, "GetHeaderMap","()Ljava/lang/Object;");
            sendHttpAsyncA = env->GetStaticMethodID(shimClass, "SendHttpAsync", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/nio/ByteBuffer;IJZ)I");
            sendHttpAsyncB = env->GetStaticMethodID(shimClass, "SendHttpStringAsync", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;IJZ)I");

            if (!raiseKeyboard) XLI_THROW("Cannot cache mid for raiseKeyboard.");
            if (!hideKeyboard) XLI_THROW("Cannot cache mid for hideKeyboard.");
            if (!getKeyboardSize) XLI_THROW("Cannot cache mid for getKeyboardSize.");
            if (!showMessageBox) XLI_THROW("Cannot cache mid for showMessageBox.");
            if (!connectedToNetwork) XLI_THROW("Cannot cache mid for connectedToNetwork.");
            if (!initDefaultCookieManager) XLI_THROW("Cannot cache mid for initDefaultCookieManager.");
            if (!getAssetManager) XLI_THROW("Cannot cache mid for getAssetManager.");
            if (!hideStatusBar) XLI_THROW("Cannot cache mid for hideStatusBar.");
            if (!showStatusBar) XLI_THROW("Cannot cache mid for showStatusBar.");
            if (!getStatusBarHeight) XLI_THROW("Cannot cache mid for getStatusBarHeight.");
            if (!getDisplayMetrics) XLI_THROW("Cannot cache mid for getDisplayMetrics.");
            if (!hasVibrator) XLI_THROW("Cannot cache mid for hasVibrator.");
            if (!vibrateForMilliseconds) XLI_THROW("Cannot cache mid for vibrateForMilliseconds.");
            if (!abortAsyncTask) XLI_THROW("Cannot cache mid for asyncTask.");
            if (!holdObject) XLI_THROW("Cannot cache mid for holdObject.");
            if (!getObject) XLI_THROW("Cannot cache mid for getObject.");
            if (!tryReleaseObject) XLI_THROW("Cannot cache mid for tryReleaseObject.");
            if (!asyncInputStreamToString) XLI_THROW("Cannot cache mid for asyncInputStreamToString.");
            if (!asyncInputStreamToByteArray) XLI_THROW("Cannot cache mid for asyncInputStreamToByteArray.");
            if (!getHeaderMap) XLI_THROW("Cannot cache mid for getHeaderMap.");
            if (!sendHttpAsyncA) XLI_THROW("Cannot cache mid for sendHttpAsyncA.");
            if (!sendHttpAsyncB) XLI_THROW("Cannot cache mid for sendHttpAsyncB.");
            LOGD("Mids Cached");
            //LOGD("out_0");
        }

        void AShim::RaiseSoftKeyboard()
        {
            // LOGD("in_RaiseSoftKeyboard");
            AJniHelper jni;
            jni->CallStaticVoidMethod(jni.GetShim(), raiseKeyboard);
            kbVisible = 1;
            // LOGD("out_RaiseSoftKeyboard");
        }

        void AShim::HideSoftKeyboard()
        {
            // LOGD("in_HideSoftKeyboard");
            AJniHelper jni;
            jclass shimClass = jni.GetShim();
            jni->CallStaticVoidMethod(shimClass, hideKeyboard);
            kbVisible = 0;
            // LOGD("out_HideSoftKeyboard");
        }

        int AShim::GetKeyboardSize()
        {
            //LOGD("in_4");
            AJniHelper jni;
            jclass shimClass = jni.GetShim();
            jint result = (jint)jni->CallStaticIntMethod(shimClass, getKeyboardSize);
            //LOGD("out_4");
            return (int)result;
        }

        void AShim::HideStatusBar()
        {
            //LOGD("in_5");
            AJniHelper jni;
            jclass shimClass = jni.GetShim();
            jni->CallStaticVoidMethod(shimClass, hideStatusBar);
            //{TODO} When this is working, go fix AWindow statusbar methods
            //LOGD("out_5");
        }

        void AShim::ShowStatusBar()
        {
            //LOGD("in_6");
            AJniHelper jni;
            jclass shimClass = jni.GetShim();
            jni->CallStaticVoidMethod(shimClass, showStatusBar);
            //{TODO} When this is working, go fix AWindow statusbar methods
            //LOGD("out_6");
        }

        float AShim::GetStatusBarHeight()
        {
            //LOGD("in_7");
            AJniHelper jni;
            jclass shimClass = jni.GetShim();
            jfloat height = jni->CallStaticFloatMethod(shimClass, getStatusBarHeight);
            //LOGD("out_7");
            return (float)height;
        }

        float AShim::GetDensity()
        {
            //LOGD("in_8");
            AJniHelper jni;
            jclass shimClass = jni.GetShim();
            jclass metricsClass = jni->FindClass("android/util/DisplayMetrics");
            jfieldID jd = jni->GetFieldID( metricsClass, "density", "F" );
            jobject result = jni->CallStaticObjectMethod(shimClass, getDisplayMetrics);
            jfloat d = (jfloat)jni->GetFloatField(result, jd);
            jni->DeleteLocalRef(result);
            jni->DeleteLocalRef(metricsClass);
            //LOGD("out_8");
            return (float)d;
        }

        Vector2 AShim::GetDpi()
        {
            //LOGD("in_9");
            AJniHelper jni;
            jclass shimClass = jni.GetShim();
            jclass metricsClass = jni->FindClass("android/util/DisplayMetrics");
            jfieldID xDpi = jni->GetFieldID( metricsClass, "xdpi", "F" );
            jfieldID yDpi = jni->GetFieldID( metricsClass, "ydpi", "F" );
            jobject result = jni->CallStaticObjectMethod(shimClass, getDisplayMetrics);
            jfloat jx = (jfloat)jni->GetFloatField(result, xDpi);
            jfloat jy = (jfloat)jni->GetFloatField(result, yDpi);
            jni->DeleteLocalRef(result);
            jni->DeleteLocalRef(metricsClass);
            //LOGD("out_9");
            return Vector2((float)jx, (float)jy);
        }

        bool AShim::KeyboardVisible()
        {
            //LOGD("in_10");
            return kbVisible;
            //LOGD("out_10");
        }

        bool AShim::HasVibrator()
        {
            //LOGD("in_11");
            AJniHelper jni;
            jclass shimClass = jni.GetShim();
            jboolean jresult = jni->CallStaticBooleanMethod(shimClass, hasVibrator);
            //LOGD("out_11");
            return (bool)jresult;
        }

        void AShim::VibrateForMilliseconds(int milliseconds)
        {
            //LOGD("in_12");
            AJniHelper jni;
            jclass shimClass = jni.GetShim();
            jni->CallStaticVoidMethod(shimClass, vibrateForMilliseconds, (jint)milliseconds);
            //LOGD("out_12");
        }

        int AShim::ShowMessageBox(const String& message, const String& caption, int buttons, int hints)
        {
            //LOGD("in_13");
            //setup for call
            AJniHelper jni;
            jclass shimClass = jni.GetShim();

            //vars for call
            jstring jcaption = jni->NewStringUTF(caption.Ptr());
            jstring jmessage = jni->NewStringUTF(message.Ptr());

            //call
            int result = (int)jni->CallStaticIntMethod(shimClass, showMessageBox, jcaption, jmessage, (jint)buttons, (jint)hints);
            //LOGD("out_13");
            return result;
        }

        bool AShim::ConnectedToNetwork()
        {
            //LOGD("in_14");
            //setup for call
            AJniHelper jni;
            jclass shimClass = jni.GetShim();
            jboolean jresult = jni->CallStaticBooleanMethod(shimClass, connectedToNetwork);
            //LOGD("out_14");
            return (bool)jresult;
        }

        JObjRef AShim::SendHttpAsync(const HttpRequest* req, const void* content, long byteLength)
        {
            //LOGD("in_15");
            AJniHelper jni;

            String url = req->GetUrl();
            String method = req->GetMethod();

            jstring jurl = jni->NewStringUTF(url.Ptr());
            jstring jmethod = jni->NewStringUTF(method.Ptr());
            jint jtimeout = (jint)req->GetTimeout();
                
            String headers = HeadersToString(req);
            jstring jheaders = jni->NewStringUTF(headers.Ptr());

            jobject arrayHandle = 0;
            if ((content!=0) && (byteLength>0))
            {
                // no way around the const_cast here, we dont modify the data in java so should be safe.
                arrayHandle = jni->NewDirectByteBuffer(const_cast<void*>(content), byteLength);
                if (arrayHandle == NULL) {
                    XLI_THROW("XliHttp - DirectByteBuffers not supported by this device.");
                }
            }

            jint jresult = jni->CallStaticIntMethod(jni.GetShim(), sendHttpAsyncA, jurl, jmethod, jheaders, arrayHandle,
                                                    jtimeout, (jlong)req, req->GetVerifyHost());
            AShim::CheckExceptions();

            jni->DeleteLocalRef(jurl);
            jni->DeleteLocalRef(jmethod);
            jni->DeleteLocalRef(jheaders);

            if (jresult==-1)
            {
                LOGE("AShim [SendHttpAsync]: Could not create async http request");
                //LOGD("out_15");
                return 0;
            }
            //LOGD("out_15");
            return (JObjRef)jresult;
        }

        void AShim::CheckExceptions()
        {
            AJniHelper jni;
            jthrowable err = jni->ExceptionOccurred();
            if (err != NULL)
            {
                jni->ExceptionDescribe();
                jni->ExceptionClear();

                jmethodID toString = jni->GetMethodID(jni->FindClass("java/lang/Object"), "toString", "()Ljava/lang/String;");
                jstring estring = (jstring) jni->CallObjectMethod(err, toString);
                jboolean isCopy;
                XLI_THROW(jni->GetStringUTFChars(estring, &isCopy));
            }
        }

        JObjRef AShim::SendHttpAsync(const HttpRequest* req, String content)
        {
            //LOGD("in_16");
            AJniHelper jni;

            String url = req->GetUrl();
            String method = req->GetMethod();

            jstring jurl = jni->NewStringUTF(url.Ptr());
            jstring jmethod = jni->NewStringUTF(method.Ptr());
            jint jtimeout = (jint)req->GetTimeout();

            String headers = HeadersToString(req);
            jstring jheaders = jni->NewStringUTF(headers.Ptr());

            jobject body = 0;

            if ((content.Length()>0))
            {
                body = jni->NewStringUTF(content.Ptr());
            }

            jint jresult = jni->CallStaticIntMethod(jni.GetShim(), sendHttpAsyncB, jurl, jmethod, jheaders, body,
                                                    jtimeout, (jlong)req, req->GetVerifyHost());

            jni->DeleteLocalRef(jurl);
            jni->DeleteLocalRef(jmethod);
            jni->DeleteLocalRef(jheaders);

            if (body!=0) jni->DeleteLocalRef(body);

            if (jresult==-1)
            {
                LOGE("AShim [SendHttpAsync]: Could not create async http request");
                //LOGD("out_16");
                return 0;
            }
            //LOGD("out_16");
            return (JObjRef)jresult;
        }

        JObjRef AShim::SendHttpAsync(const HttpRequest* req)
        {
            // LOGD("in_17");
            AJniHelper jni;

            String url = req->GetUrl();
            String method = req->GetMethod();

            jstring jurl = jni->NewStringUTF(url.Ptr());
            jstring jmethod = jni->NewStringUTF(method.Ptr());
            jint jtimeout = (jint)req->GetTimeout();
            jobject arrayHandle = 0;

            String headers = HeadersToString(req);
            jstring jheaders = jni->NewStringUTF(headers.Ptr());

            jint jresult = jni->CallStaticIntMethod(jni.GetShim(), sendHttpAsyncA, jurl, jmethod, jheaders, arrayHandle,
                                                    jtimeout, (jlong)req, req->GetVerifyHost());
            jni->DeleteLocalRef(jurl);
            jni->DeleteLocalRef(jmethod);
            jni->DeleteLocalRef(jheaders);
            if (jresult==0)
            {
                LOGE("AShim [SendHttpAsync]: Could not create async http request");
                // LOGD("out_17");
                return 0;
            }
            // LOGD("out_17");
            return (JObjRef)jresult;
        }

        void AShim::AbortAsyncTask(JObjRef task)
        {
            //LOGD("in_18");
            AJniHelper jni;
            jni->CallStaticVoidMethod(jni.GetShim(), abortAsyncTask, (jint)task);
            //LOGD("out_18");
        }

        String AShim::HeadersToString(const HttpRequest* req)
        {
            //LOGD("in_19");
            String result = "";
            AJniHelper jni;
            int i = req->HeadersBegin();
            while (i != req->HeadersEnd())
            {
                result += req->GetHeaderKey(i) + ":" + req->GetHeaderValue(i) + "\n";
                i = req->HeadersNext(i);
            }
            //LOGD("out_19");
            return result;
        }

        String AShim::InputStreamToString(jobject bufferedInputStream)
        {
            //LOGD("in_20");
            AJniHelper jni;
            jclass shimClass = jni.GetShim();
            jmethodID mid = jni->GetStaticMethodID(shimClass, "InputStreamToString", "(Ljava/io/InputStream;)Ljava/lang/String;");
            if (!mid) {
                LOGE("Unable to get InputStreamToString mid");
                //LOGD("out_20");
                return "";
            }
            jstring jresult = (jstring)jni->CallStaticObjectMethod(shimClass, mid, bufferedInputStream,(jint)1024);
            String result = jni.GetString(jresult);
            jni->DeleteLocalRef(jresult);
            //LOGD("out_20");
            return result;
        }

        JObjRef AShim::AsyncInputStreamToString(JObjRef bufferedInputStream, HttpRequest* request)
        {
            //LOGD("in_21");
            AJniHelper jni;
            jint result = jni->CallStaticIntMethod(jni.GetShim(), asyncInputStreamToString, (jint)bufferedInputStream, (long)request);
            //LOGD("in_21");
            if (result==-1) return 0;
            return (JObjRef)result;
        }

        JObjRef AShim::AsyncInputStreamToByteArray(JObjRef bufferedInputStream, HttpRequest* request)
        {
            //LOGD("in_22");
            AJniHelper jni;
            jint result = jni->CallStaticIntMethod(jni.GetShim(), asyncInputStreamToByteArray, (jint)bufferedInputStream, (long)request);
            //LOGD("in_22");
            if (result == -1) return 0;
            return (JObjRef)result;
        }

        int AShim::ReadBytesFromInputStream(jobject bufferedInputStream, int bytesToRead, void* dst)
        {
            //LOGD("in_23");
            AJniHelper jni;
            jclass shimClass = jni.GetShim();

            jmethodID mid = jni->GetStaticMethodID(shimClass, "ReadBytesFromInputStream", "(Ljava/io/BufferedInputStream;I)[B");
            if (!mid) {
                LOGE("Unable to get ReadBytesFromInputStream mid");
                //LOGD("out_23");
                return 0;
            }
            jbyteArray jbytes = (jbyteArray)jni->CallStaticObjectMethod(shimClass, mid, bufferedInputStream, (jint)bytesToRead);
            if (jbytes != 0) {
                jsize len = jni->GetArrayLength(jbytes);
                jni->GetByteArrayRegion(jbytes, 0, len, (jbyte*)dst);
                int result = (int)len;
                jni->DeleteLocalRef(jbytes);
                //jni->DeleteLocalRef((jobject)len);
                //LOGD("out_23");
                return result;
            } else {
                //LOGD("out_23");
                return -1;
            }
            //LOGD("out_23");
        }

        void AShim::InitDefaultCookieManager()
        {
            //LOGD("in_24");
            AJniHelper jni;
            jclass shimClass = jni.GetShim();
            jni->CallStaticVoidMethod(shimClass, initDefaultCookieManager);
            //LOGD("out_24");
        }


        AAssetManager* AShim::GetAssetManager()
        {
            //LOGD("in_25");
            AJniHelper jni;
            jclass shimClass = jni.GetShim();
            jobject assetManager = jni->CallStaticObjectMethod(shimClass, getAssetManager);
            jni->NewGlobalRef(assetManager);
            AAssetManager* result = AAssetManager_fromJava(jni.GetEnv(), assetManager);
            //LOGD("out_25");
            return result;
        }

        bool AShim::RegisterNativeFunctions(JNINativeMethod native_funcs[], int funcCount)
        {
            //LOGD("in_26");
            AJniHelper jni;
            jclass shimClass = jni.GetShim();
            jint attached = jni->RegisterNatives(shimClass, native_funcs, (jint)funcCount);
            //LOGD("out_26");
            return (attached >= 0);
        }

        JObjRef AShim::HoldObject(jobject obj)
        {
            //LOGD("in_27");
            AJniHelper jni;
            jint result = jni->CallStaticIntMethod(jni.GetShim(), holdObject, obj);
            //LOGD("out_27");
            return (JObjRef)result;
        }

        jobject AShim::GetObject(JObjRef objKey)
        {
            //LOGD("in_27");
            AJniHelper jni;
            jobject result = jni->CallStaticObjectMethod(jni.GetShim(), getObject, (jint)objKey);
            //LOGD("out_27");
            return result;
        }

        bool AShim::TryReleaseObject(JObjRef objKey)
        {
            //LOGD("in_28");
            AJniHelper jni;
            bool result = (bool)jni->CallStaticBooleanMethod(jni.GetShim(), tryReleaseObject, (jint)objKey);
            //LOGD("out_28");
            return result;
        }

        jobject AShim::GetHeaderMap()
        {
            //LOGD("in_28");
            AJniHelper jni;
            jobject result = jni->CallStaticObjectMethod(jni.GetShim(), getHeaderMap);
            //LOGD("out_28");
            return result;
        }
    }
}
