//
// Created by donnell on 1/18/17.
//
#include "HttpPost.h"

HttpPost::HttpPost() : JavaClass() {}

/**
* Here you can construct the HttpPost object how ever you need to,
* as well as load signatures for the Java instance method calls.
*/
HttpPost::HttpPost(JNIEnv *env, string url) : JavaClass(env)
{
    initialize(env);

    jstring jUrl = env->NewStringUTF(url.c_str());
    // WE HAVE ADDED A jstring url PARAMETER TO THE 'HttpPost' constructor
    thisObj = env->NewObject(_clazz, getJavaMethod(env, "<init>"), jUrl);

    if (thisObj == NULL) {
        JavaExceptionUtils::throwExceptionOfType(env, kTypeIllegalStateException,
                                                 "HttpPost's thisObj variable not intialized, methods of this class use the thisObj Java instance.");
    }
}

void HttpPost::initialize(JNIEnv *env)
{
    setClass(env);
    cacheConstructor(env);

    addJavaSignature("<init>", "(Ljava/lang/String;)V");
    addJavaSignature("getMethod", "()Ljava/lang/String;");
    addJavaSignature("setEntity", "(Ljava/lang/String;)V");
    addJavaSignature("setHeader", "(Ljava/lang/String;Ljava/lang/String;)V");

    registerNativeMethods(env);
}

void HttpPost::mapFields()
{
    //mapField("encodedString", kTypeString, &encodedString);
}

jstring HttpPost::getMethod(JNIEnv *env)
{
    jobject result = env->CallObjectMethod(thisObj, getJavaMethod(env, __FUNCTION__));
    JavaExceptionUtils::checkException(env);
    return (jstring) result;
}

void HttpPost::setEntity(JNIEnv *env, ByteArrayEntity entity)
{
    env->CallVoidMethod(thisObj, getJavaMethod(env, __FUNCTION__));
    JavaExceptionUtils::checkException(env);
}

void HttpPost::setHeader(JNIEnv *env, string key, string value)
{
    env->CallVoidMethod(thisObj, getJavaMethod(env, __FUNCTION__));
    JavaExceptionUtils::checkException(env);
}