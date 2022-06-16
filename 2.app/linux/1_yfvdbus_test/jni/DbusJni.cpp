/*****************************************************************************
*                                                                            *
*              CONFIDENTIAL VISTEON CORPORATION                              *
*                                                                            *
* This is an unpublished work of authorship, which contains trade            *
* secrets, created in 2013. Visteon Corporation owns all rights to           *
* this work and intends to maintain it in confidence to preserve             *
* its trade secret status. Visteon Corporation reserves the right,           *
* under the copyright laws of the United States or those of any              *
* other country that may have jurisdiction, to protect this work             *
* as an unpublished work, in the event of an inadvertent or                  *
* deliberate unauthorized publication. Visteon Corporation also              *
* reserves its rights under all copyright laws to protect this               *
* work as a published work, when appropriate. Those having access            *
* to this work may not copy it, use it, modify it or disclose the            *
* information contained in it without the written authorization              *
* of Visteon Corporation.                                                    *
*                                                                            *
******************************************************************************/
/*
 * =====================================================================================
 *
 *    Filename:  DbusJni.cpp
 *
 *    Description:
 *
 *    Version:  1.0
 *    Created:  10/22/2018 09:35:07 AM
 *
 *    Author:  Deep Lee
 *  =====================================================================================
 *
 */

#include <CommAdaptorNativeService.h>
#include <JNIHelp.h>
#include <android_runtime/AndroidRuntime.h>
#include <binder/IServiceManager.h>
#include <jni.h>
#include <unistd.h>
#include <utils/Log.h>
#include <utils/threads.h>

#define LOG_TAG ("DbusJni")

using namespace android;

static const char* const kClassJavaDbus = "com/yfvet/javadbus/Dbus";

struct fields_t {
    jfieldID context;
    jmethodID post_event;
};

static fields_t JavaDbusfields;
static Mutex sLock;

class NativeReceiveCallback : public BnReceiveNativeCallback {
public:
    NativeReceiveCallback(JNIEnv* env, jobject thiz, jobject weak_thiz);
    ~NativeReceiveCallback();
    virtual bool OnReceive(const char* ifName, const char* mbName, size_t len, const uint8_t* pl);

private:
    NativeReceiveCallback();
    jclass mClass;
    jobject mObject;
};

NativeReceiveCallback::NativeReceiveCallback(JNIEnv* env, jobject thiz, jobject weak_thiz) {
    ALOGD("oncreat");
    jclass clazz = env->GetObjectClass(thiz);
    if (clazz == NULL) {
        ALOGE("cann't find clazz : %s", kClassJavaDbus);
        jniThrowException(env, "java/lang/Exception", NULL);
        return;
    }
    mClass = (jclass)env->NewGlobalRef(clazz);
    mObject = env->NewGlobalRef(weak_thiz);
}

NativeReceiveCallback::~NativeReceiveCallback() {
    ALOGD("ondestory");
    JNIEnv* env = AndroidRuntime::getJNIEnv();
    // env->DeleteGlobalRef( mObject  );
    env->DeleteGlobalRef(mClass);
}

bool NativeReceiveCallback::OnReceive(const char* ifName, const char* mbName, size_t len, const uint8_t* pl) {
    ALOGD("Native Received ifName=%s, mbName=%s", ifName, mbName);
    JNIEnv* env = AndroidRuntime::getJNIEnv();
    jstring strIfName = env->NewStringUTF(ifName);
    jstring strMbName = env->NewStringUTF(mbName);
    jbyteArray arrPl = env->NewByteArray(len);
    env->SetByteArrayRegion(arrPl, 0, len, (const jbyte*)(pl));

    jboolean bret =
        env->CallBooleanMethod(mClass, JavaDbusfields.post_event, mObject, strIfName, strMbName, (jint)len, arrPl);
    ALOGD("env->CallBooleanMethod ret=%d", bret ? 0 : 1);
    return bret;
}

static sp<ICommAdaptorNative> getCommAdaptorNative(JNIEnv* env, jobject thiz) {
    Mutex::Autolock l(sLock);
    ICommAdaptorNative* const p = (ICommAdaptorNative*)env->GetLongField(thiz, JavaDbusfields.context);
    ALOGD("getCommAdaptorNative");
    return sp<ICommAdaptorNative>(p);
}

static sp<ICommAdaptorNative> setCommAdaptorNative(JNIEnv* env, jobject thiz, const sp<ICommAdaptorNative>& comm) {
    Mutex::Autolock l(sLock);
    sp<ICommAdaptorNative> old = (ICommAdaptorNative*)env->GetLongField(thiz, JavaDbusfields.context);
    if (comm.get()) {
        comm->incStrong((void*)setCommAdaptorNative);
    }

    if (old != 0) {
        old->decStrong((void*)setCommAdaptorNative);
    }

    env->SetLongField(thiz, JavaDbusfields.context, (jlong)comm.get());
    ALOGD("setCommAdaptorNative");
    return old;
}

void native_Init(JNIEnv* env, jobject instance) {
    jclass clazz;
    clazz = env->FindClass(kClassJavaDbus);
    if (clazz == NULL) {
        return;
    }

    JavaDbusfields.context = env->GetFieldID(clazz, "mNativeContext", "J");
    if (JavaDbusfields.context == NULL) {
        ALOGE("context is NULL");
        return;
    }

    JavaDbusfields.post_event =
        env->GetMethodID(clazz, "OnReceive", "(Ljava/lang/Object;Ljava/lang/String;Ljava/lang/String;I[B)Z");
    if (JavaDbusfields.post_event == NULL) {
        ALOGE("post_event is NULL");
        return;
    }
    env->DeleteLocalRef(clazz);
}

void native_Setup(JNIEnv* env, jobject thiz) {
    sp<IServiceManager> sm = defaultServiceManager();
    sp<IBinder> binder = sm->getService(ICommAdaptorNative::descriptor);
    if (binder == NULL) {
        ALOGD("getService(%s) fail!!", String8(ICommAdaptorNative::descriptor).string());
        return;
    }
    sp<ICommAdaptorNative> comm = interface_cast<ICommAdaptorNative>(binder);
    if (comm == NULL) {
        ALOGE("No ICommAdaptorNative in %s !!", String8(ICommAdaptorNative::descriptor).string());
        return;
    }
    ALOGD("get ICommAdaptorNative ok. pid=%d", getpid());
    setCommAdaptorNative(env, thiz, comm);
}

jboolean native_Send(JNIEnv* env, jobject thiz, jstring ifName_, jstring mbName_, jint len, jbyteArray pl_) {
    sp<ICommAdaptorNative> comm = getCommAdaptorNative(env, thiz);
    if (comm == NULL) {
        ALOGE("Error! comm = %s", "NULL");
        return JNI_FALSE;
    }
    const char* ifName = env->GetStringUTFChars(ifName_, 0);
    const char* mbName = env->GetStringUTFChars(mbName_, 0);
    jbyte* pl = env->GetByteArrayElements(pl_, NULL);
    if (NULL == pl) {
        LBL_LOGE("GetByteArrayElements(0x%x) return NULL!", pl_);
        return JNI_FALSE;
    }
    bool bret = comm->Send(ifName, mbName, len, (uint8_t*)(pl));
    env->ReleaseStringUTFChars(ifName_, ifName);
    env->ReleaseStringUTFChars(mbName_, mbName);
    env->ReleaseByteArrayElements(pl_, pl, 0);
    return bret ? JNI_TRUE : JNI_FALSE;
}

jboolean native_AddReceiveCallback(JNIEnv* env, jobject thiz, jobject weak_thiz, jstring ifName_) {
    sp<ICommAdaptorNative> comm = getCommAdaptorNative(env, thiz);
    if (comm == NULL) {
        ALOGE("Error! comm = %s", "NULL");
        return JNI_FALSE;
    }
    sp<NativeReceiveCallback> mCallback = new NativeReceiveCallback(env, thiz, weak_thiz);
    if (mCallback == NULL) {
        ALOGE("Error! Cannot new NativeReceiveCallback");
        return JNI_FALSE;
    }
    const char* ifName = env->GetStringUTFChars(ifName_, 0);
    bool bret = comm->AddReceiveCallback(ifName, mCallback);
    env->ReleaseStringUTFChars(ifName_, ifName);
    return bret ? JNI_TRUE : JNI_FALSE;
}

jboolean native_DelReceiveCallback(JNIEnv* env, jobject thiz, jstring ifName_) {
    sp<ICommAdaptorNative> comm = getCommAdaptorNative(env, thiz);
    if (comm == NULL) {
        ALOGE("Error! ICommAdaptorNative = %s", "NULL");
        return JNI_FALSE;
    }
    const char* ifName = env->GetStringUTFChars(ifName_, 0);
    bool bret = comm->DelReceiveCallback(ifName);
    env->ReleaseStringUTFChars(ifName_, ifName);
    return bret ? JNI_TRUE : JNI_FALSE;
}

static JNINativeMethod gMethods[] = {
    { "nativeInit", "()V", (void*)native_Init },
    { "nativeSetup", "()V", (void*)native_Setup },
    { "nativeSend", "(Ljava/lang/String;Ljava/lang/String;I[B)Z", (void*)native_Send },
    { "nativeAddCallback", "(Ljava/lang/Object;Ljava/lang/String;)Z", (void*)native_AddReceiveCallback },
    { "nativeDelCallback", "(Ljava/lang/String;)Z", (void*)native_DelReceiveCallback },
};

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    ALOGD("####  DbusJni OnLoad ####");
    JNIEnv* env = NULL;
    jint result = JNI_FALSE;

    if (vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK) {
        ALOGE("Can not get env");
        return result;
    }

    if (env == NULL) {
        ALOGE("env is null");
        return result;
    }

    jclass clazz = env->FindClass(kClassJavaDbus);
    if (clazz == NULL) {
        ALOGE("can not find class -> %s", kClassJavaDbus);
        return result;
    }

    if (env->RegisterNatives(clazz, gMethods, NELEM(gMethods)) < 0) {
        ALOGE("register error");
        return result;
    }

    result = JNI_VERSION_1_4;
    return result;
}
