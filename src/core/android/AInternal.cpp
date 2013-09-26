#include "AInternal.h"
#include <pthread.h>
#include <stdarg.h>

namespace Xli
{
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
		pthread_setspecific(JniThreadKey, NULL);
	}

    int JniHelper::shim_loaded = 0;

	void JniHelper::Init()
	{
		if (pthread_key_create(&JniThreadKey, JniDestroyThread))
			LOGE("JNI ERROR: Unable to create pthread key"); // Not fatal
		if (pthread_key_create(&JniShimKey, JniDestroyShim))
			LOGE("JNI ERROR: Unable to Shim pthread key"); // Not fatal
	}

	JniHelper::JniHelper()
	{
		if (AndroidActivity->vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
		{
			if (AndroidActivity->vm->AttachCurrentThread(&env, NULL) != JNI_OK)
            {
				XLI_THROW("JNI ERROR: Failed to attach current thread");
            } else {

                if (!shim_loaded) {
                    LOGD("Loading shim");
                    JniHelper::shim_loaded = PrepareAssetJar("XliShimJ.apk","XliJ");
                }

                jclass *shim_class = new jclass;
                *shim_class = GetAssetClass("XliShimJ.apk","XliJ");
                pthread_setspecific(JniThreadKey, (void*)env);
                pthread_setspecific(JniShimKey, (void*)shim_class);
            }
		}
        jclass* shim_p = reinterpret_cast<jclass*>(pthread_getspecific(JniShimKey));
        if (shim_p)
        {
            shim = *shim_p;
        } else {
            LOGE("could not get shim");
        }
	}

    jclass JniHelper::GetShim()
    {
        if (!shim_loaded) LOGE("Shim isnt loaded yet");
        return shim;
    }

	jmethodID JniHelper::FindMethod(const char* className, const char* methodName, const char* methodSig)
	{
		jclass cls = env->FindClass(className);
		if (!cls) XLI_THROW((String)"Failed to get JNI class '" + className + "'");

		jmethodID method = env->GetMethodID(cls, methodName, methodSig);
		if (!method) XLI_THROW((String)"Failed to get JNI method '" + className + "." + methodName + methodSig + "'");

		return method;
	}

	jobject JniHelper::CallObjectMethod(jobject inst, const char* name, const char* sig)
	{
		jclass cls = env->GetObjectClass(inst);

#ifdef DEBUG_JNI
		if (!cls) XLI_THROW((String)"Failed to get JNI class for method '" + name + "'");
#endif

		jmethodID method = env->GetMethodID(cls, name, sig);

#ifdef DEBUG_JNI
		if (!method) XLI_THROW((String)"Failed to get JNI method '" + name + "'");
#endif

		return env->CallObjectMethod(inst, method);
	}

	String JniHelper::GetString(jobject str)
	{
		const char* utf8 = env->GetStringUTFChars((jstring)str, NULL);
		String result = utf8;
		env->ReleaseStringUTFChars((jstring)str, utf8);
		return result;
	}

	JNIEnv* JniHelper::GetEnv()
	{
		// TODO: Check jni exceptions
		return env;
	}

	JNIEnv* JniHelper::operator->()
	{
		// TODO: Check jni exceptions
		return env;
	}
    
    jmethodID JniHelper::GetInstanceMethod(const char* m_name, const char* m_sig)
    {
        return GetInstanceMethod(GetInstance(), m_name, m_sig);
    }

    jmethodID JniHelper::GetInstanceMethod(jobject inst, const char* m_name, const char* m_sig)
    {
        return env->GetMethodID(env->GetObjectClass(inst), m_name, m_sig);
    }

    jobject JniHelper::GetInstance() 
    {
        return AndroidActivity->clazz;
    }

    jobject JniHelper::GetInstance(const char* class_name, const char* constructor_sig, ...)
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

    jobject JniHelper::GetInstance(jclass cls, const char* constructor_sig, ...)
    {
        jmethodID mid = env->GetMethodID(cls, "<init>", constructor_sig);
        jobject result = 0;
        va_list args;
        va_start(args, constructor_sig);
        result = env->NewObjectV(cls, mid, args);
        va_end(args);
        return result;
    }

    jclass JniHelper::GetCustomClass(const char* class_path)
    {
        jclass activityClass = env->FindClass("android/app/NativeActivity");
        jmethodID getClassLoader = env->GetMethodID(activityClass,"getClassLoader", "()Ljava/lang/ClassLoader;");
        jobject cls = env->CallObjectMethod(AndroidActivity->clazz, getClassLoader);
        jclass classLoader = env->FindClass("java/lang/ClassLoader");
        jmethodID findClass = env->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
        jstring strClassName = env->NewStringUTF(class_path);
        jclass classIWant = (jclass)env->CallObjectMethod(cls, findClass, strClassName);
    }

    int JniHelper::PrepareAssetJar(const char* file_name, const char* class_name) 
    {
        return PrepareAssetJar(file_name, class_name, 1);
    }

    int JniHelper::PrepareAssetJar(const char* file_name, const char* class_name, int package)
    {
        jmethodID get_dir = GetInstanceMethod("getDir","(Ljava/lang/String;I)Ljava/io/File;");

        //dex-file
        jobject dex_dir_file = env->CallObjectMethod(AndroidActivity->clazz, get_dir, env->NewStringUTF("dex"), (jint)0);

        //input stream 
        jstring jfile_name = env->NewStringUTF(file_name);
        jobject dex_file = GetInstance("java/io/File","(Ljava/io/File;Ljava/lang/String;)V", dex_dir_file, jfile_name);
        

        //streams
        jmethodID get_as_mid = GetInstanceMethod("getAssets","()Landroid/content/res/AssetManager;");
        jobject asset_mgr = env->CallObjectMethod(AndroidActivity->clazz, get_as_mid);
        jmethodID amgr_open = GetInstanceMethod(asset_mgr, "open","(Ljava/lang/String;)Ljava/io/InputStream;");
        jclass bis_cls = env->FindClass("java/io/BufferedInputStream");
        jobject bis = env->CallObjectMethod(asset_mgr, amgr_open, jfile_name);
        jobject fos = GetInstance("java/io/FileOutputStream","(Ljava/io/File;)V", dex_file);
        jclass dexWriter_cls = env->FindClass("java/io/BufferedOutputStream");
        jobject dexWriter = GetInstance("java/io/BufferedOutputStream","(Ljava/io/OutputStream;)V", fos);

        //copy
        jint BUFSIZE = 8 * 1024;
        jint len = 0;
        jbyteArray bArray = env->NewByteArray(BUFSIZE);
        jmethodID bis_read = env->GetMethodID(bis_cls,"read","([BII)I");
        jmethodID dex_write = env->GetMethodID(dexWriter_cls,"write","([BII)V");
        
        len = (jint)env->CallObjectMethod(bis, bis_read, bArray, 0, BUFSIZE);
        while ((int) len > 0) {       
            env->CallObjectMethod(dexWriter, dex_write, bArray, 0, BUFSIZE);
            len = (jint)env->CallObjectMethod(bis, bis_read, bArray, 0, BUFSIZE);
        }

        return 1;
    }

    jclass JniHelper::GetAssetClass(const char* file_name, const char* class_name)
    {
        jmethodID get_dir = GetInstanceMethod("getDir","(Ljava/lang/String;I)Ljava/io/File;");

        jobject dex_dir_file = env->CallObjectMethod(AndroidActivity->clazz, get_dir, env->NewStringUTF("dex"), (jint)0);
        jclass file_cls = env->FindClass("java/io/File");
        jmethodID getAbsPath = env->GetMethodID(file_cls, "getAbsolutePath", "()Ljava/lang/String;");

        jclass dexloader_cls = env->FindClass("dalvik/system/DexClassLoader");
        jmethodID load_class = env->GetMethodID(dexloader_cls, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
        jmethodID get_classloader = GetInstanceMethod("getClassLoader","()Ljava/lang/ClassLoader;");
        jobject classloader = env->CallObjectMethod(AndroidActivity->clazz, get_classloader);

        jobject dex_file = GetInstance("java/io/File","(Ljava/io/File;Ljava/lang/String;)V", dex_dir_file, env->NewStringUTF(file_name));
        jobject outdex_dir_file = env->CallObjectMethod(AndroidActivity->clazz, get_dir, env->NewStringUTF("outdex"), (jint)0);
        jstring dex_optimized_out_path = (jstring)env->CallObjectMethod(outdex_dir_file, getAbsPath);
        jstring dex_internal_path = (jstring)env->CallObjectMethod(dex_file, getAbsPath);

        jobject dexloader = GetInstance("dalvik/system/DexClassLoader", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)V", dex_internal_path, dex_optimized_out_path, NULL, classloader);

        jclass loaded = (jclass)env->CallObjectMethod(dexloader, load_class, env->NewStringUTF(class_name));
        jclass globalClass = reinterpret_cast<jclass>(env->NewGlobalRef(loaded));
        return globalClass;
    }
}
