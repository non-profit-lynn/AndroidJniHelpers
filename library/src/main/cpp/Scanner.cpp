//
// Created by Christopher Miller on 1/12/17.
//

#include "Scanner.h"


void Scanner::initialize(JNIEnv *env)
{
    setClass(env);

    cacheSignature(env, "<init>", "(Ljava/io/InputStream;)V");
    cacheSignature(env, "useDelimiter", "(Ljava/lang/String;)Ljava/util/Scanner;");
    cacheSignature(env, "next", "()Ljava/lang/String;");

    registerNativeMethods(env);
}

void Scanner::mapFields()
{
//    mapField("requestUrl", kTypeString, &requestUrl);
}

void Scanner::useDelimiter(JNIEnv *env, std::string delimit)
{
    jstring value = env->NewStringUTF(delimit.c_str());
    jobject result = env->CallObjectMethod(thisObj, getMethod(__FUNCTION__), value);
    JavaExceptionUtils::checkException(env);
}

jstring Scanner::next(JNIEnv *env)
{
    jobject result = env->CallObjectMethod(thisObj, getMethod(__FUNCTION__));
    JavaExceptionUtils::checkException(env);
    return (jstring) result;
}