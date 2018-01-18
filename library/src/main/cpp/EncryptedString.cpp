//
// Created by Christopher Miller on 1/14/17.
//

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include "SharedPreferences.h"
#include "EncryptedString.h"
#include "Scanner.h"
#include "CryptoHelper.h"
#include "AndroidJniApp.h"
#include "FileOutputStream.h"

using namespace std;

static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                "0123456789+/";

static vector<int> *elems = new vector<int>();

EncryptedString::EncryptedString() : NativeObject() {}

/**
* This constructor is the loader for the Java instance used to call all of its methods.
* You can construct the EncryptedString object how ever you need to here.
*/
EncryptedString::EncryptedString(JNIEnv *env) : NativeObject(env)
{
    initialize(env);

    thisObj = toJavaObject(env); // THIS IS WHERE YOU INITIALIZE YOUR JAVA OBJECT, YOU MIGHT WANT TO ADD A FEW PARAMETERS BELOW
//    thisObj = env->NewObject(_clazz, getMethod(getCanonicalName())); // CALLS DEFAULT CONSTRUCTOR

//    ByteArray encodedKey = ByteArray(env, JavaString(getKey(env, NATIVE_STRINGS_ALGORITHM)).toByteArray(env));
//    ByteArray decodedKey = ByteArray(env, Base64::decodeBase64( env, encodedKey.toJavaByteArray(env)));
//    crypto = CryptoHelper(env, decodedKey.toJavaByteArray(env));
//    crypto = CryptoHelper(env, EncryptedString::getBytes(env, thisObj));

    if (thisObj == NULL) {
        JavaExceptionUtils::throwExceptionOfType(env, kTypeIllegalStateException,
                                                 "EncryptedString's thisObj variable not intialized, methods of this class use the thisObj Java instance.");
    }
}
void EncryptedString::initialize(JNIEnv *env)
{
    setClass(env);
    cacheConstructor(env);
    cacheField(env, "encryptedString", kTypeString);

    cacheSignature(env, "equals", "(Ljava/lang/Object;)Z");
    cacheSignature(env, "toString", "()Ljava/lang/String;");
    cacheSignature(env, "getFileStream", "(Ljava/lang/String;)Ljava/io/InputStream;");
    addNativeSignature("decrypt", (void*)&EncryptedString::decrypt, "(I)Ljava/lang/String;");

    // addNativeMethod("destroy", (void*)&EncryptedString::nativeDestroy, kTypeVoid, NULL);
    registerNativeMethods(env);
}
void EncryptedString::mapFields()
{
    mapField("encryptedString", kTypeString, &encryptedString);
}

jboolean EncryptedString::equals(JNIEnv *env, jobject jobjectValue1)
{
    jboolean result = env->CallBooleanMethod(thisObj, getMethod(__FUNCTION__), jobjectValue1);
    JavaExceptionUtils::checkException(env);
    return result;
}

jstring EncryptedString::toString(JNIEnv *env)
{
    jobject result = env->CallObjectMethod(thisObj, getMethod(__FUNCTION__));
    JavaExceptionUtils::checkException(env);
    return (jstring) result;
}

jstring EncryptedString::decryptNative(JNIEnv *env, jint algorithm)
{
    return EncryptedString::decrypt(env, thisObj, algorithm);
}

jbyteArray EncryptedString::getBytes(JNIEnv *env, jobject java_this) {
    EncryptedString *object = gClasses.getNativeInstance<EncryptedString>(env, java_this);

    if (object != NULL) {
//        byte [] decodedKey = Base64.decodeBase64( getBytes() );
//        ByteArray encodedKey = ByteArray(env, JavaString(object->getKey(env, NATIVE_STRINGS_ALGORITHM)).toByteArray(env));
//        ByteArray decodedKey = ByteArray(env, Base64::decodeBase64( env, encodedKey.toJavaByteArray(env) ));
//
//        return decodedKey.toJavaByteArray(env);

//        jstring key = env->NewStringUTF("key_file");
//        Scanner s = Scanner(env, File(env, object->getFilesDir(env), key));
//        s.useDelimiter(env, "\\A");

//        const char *certificate = env->GetStringUTFChars(s.next(env), JNI_FALSE);
        const char *certificate = object->getKey(env, NATIVE_STRINGS_ALGORITHM).c_str();
        int size = std::strlen(certificate);

        jbyte *data = (jbyte *) certificate;
        jbyteArray array = env->NewByteArray(size);
        env->SetByteArrayRegion(array, 0, size, data);
        return array;
    }
    return NULL;
}
string rotCharacters(string s, int r)
{
    stringstream ss;
    for (int i = 0; i < s.length(); i++)
    {
        char c = s[i];
        if (c >= 'a' && c <= 'z') ss << (char)((c - 'a' + r) % 26 + 'a');
        else if (c >= 'A' && c <= 'Z') ss << (char)((c - 'A' + r) % 26 + 'A');
        else ss << c;
    }
    return ss.str();
}

string base_64_encode(string const &in)
{
    std::string out;

    std::vector<int> T(256,-1);
    for (int i=0; i<64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

    int val=0, valb=-8;
    for (char c : in) {
        if (T[c] == -1) break;
        val = (val<<6) + T[c];
        valb += 6;
        if (valb>=0) {
            out.push_back(char((val>>valb)&0xFF));
            valb-=8;
        }
    }
    return out;
}

//char key_map(string const& k, char const& in) {
//    int position = k.length() / 2;
//    std::size_t index = k.find(in);
//    return (char) k[position + index];
//}

//string substitution(string const& k, string const& in)
//{
//    std::string out;
//    int length = in.length();
//    for (int i = 0; i < length; i++)
//    {
//        char o = in[i];
//        char r = o;
//
//        if (o >= 'a' && o <= 'z' || o >= 'A' && o <= 'Z')
//            r = key_map(k, o);
//
//        out.push_back(r);
//    }
//    return out;
//}

//jobject EncryptedString::getFileStream(JNIEnv *env, jstring fileName)
//{
//    jobject result = env->CallObjectMethod(thisObj, getMethod(__FUNCTION__), fileName);
//    JavaExceptionUtils::checkException(env);
//    return result;
//}

jobject EncryptedString::getFileStream(JNIEnv *env, jstring fileName)
{
    AndroidJniApp context(env, AndroidJniApp::Instance(env));

    JavaExceptionUtils::checkException(env);
    return context.openAssetFileInput(env, fileName);
//    return context.openFileInput(env, fileName);
}

string EncryptedString::getKey(JNIEnv *env, jint algorithm)
{
    switch(algorithm) {
    case RESOURCE_STRINGS_ALGORITHM:
        return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    case NATIVE_STRINGS_ALGORITHM:
#if TESTING_DEMO
        AndroidJniApp context(env, AndroidJniApp::Instance(env));
        SharedPreferences prefs(env, context.thisObj);
        jstring tokenKey = env->NewStringUTF("apiKey");
        return env->GetStringUTFChars(prefs.getString(env, tokenKey, NULL), JNI_FALSE);
#else
        jstring native_key = env->NewStringUTF("native_key");
        Scanner s = Scanner(env, getFileStream(env, native_key));
        s.useDelimiter(env, "\\A");
        return env->GetStringUTFChars(s.next(env), JNI_FALSE);
#endif
    }
}

jstring EncryptedString::decrypt(JNIEnv *env, jobject java_this, jint algorithm)
{
    EncryptedString *object = gClasses.getNativeInstance<EncryptedString>(env, java_this);
    if (object != NULL) {

        std::string key;
        std::string conversion;

        switch(algorithm) {
        case INLINE_STRINGS_ALGORITHM:
            conversion = rotCharacters(object->encryptedString.get(), -1);
            break;

        case RESOURCE_STRINGS_ALGORITHM:
            conversion = base_64_encode(base_64_encode(object->encryptedString.get()));
            break;

        case NATIVE_STRINGS_ALGORITHM:

            CryptoHelper cryptoHelper(env);
            cryptoHelper.setBytes(object->getBytes(env, java_this));
            return cryptoHelper.decrypt(env, object->encryptedString.toJavaString(env));

//            const char* bytes = object->getKey(env, NATIVE_STRINGS_ALGORITHM).c_str();
//            int size = std::strlen(bytes);
//
//            jbyte *data = (jbyte *) bytes;
//            jbyteArray keyBytes = env->NewByteArray(size);
//            env->SetByteArrayRegion(keyBytes, 0, size, data);
//
//            bytes = object->encryptedString.get().c_str();
//            size = std::strlen(bytes);
//
//            data = (jbyte *) bytes;
//            jbyteArray valueBytes = env->NewByteArray(size);
//            env->SetByteArrayRegion(valueBytes, 0, size, data);

//            jbyteArray decodedKeyBytes = Base64::decodeBase64( env, keyBytes );

//        case RESOURCE_STRINGS_ALGORITHM:
//            key = object->getKey(env, RESOURCE_STRINGS_ALGORITHM);
//            conversion = substitution(key, base64(object->encryptedString.get()));
//            break;//return object->encryptedString.toJavaString(env);//

//        case RESOURCE_STRINGS_ALGORITHM:
////            object->encryptedString = JavaString(base64(object->encryptedString.get()));
//
//
//            conversion = object->getKey(env, NATIVE_STRINGS_ALGORITHM);
//            ByteArray encodedKey = ByteArray(env, JavaString(conversion).toByteArray(env));
//
////            ByteArray encodedKey = ByteArray(env, JavaString(object->getKey(env, NATIVE_STRINGS_ALGORITHM)).toByteArray(env));
////            object->crypto.setBytes(env, encodedKey);
//
////            jbyteArray bytes = object->crypto.decryptNative(env, object->encryptedString.toByteArray(env));
////            conversion = JavaString(env, bytes).get();
//
////            conversion = object->encryptedString.get();
//            break;

        }

        jstring result = env->NewStringUTF(conversion.c_str());
        JavaExceptionUtils::checkException(env);
        return result;
    }
    return NULL;
}

jstring EncryptedString::getS(JNIEnv *env, jint index) {

    jstring strings = env->NewStringUTF(".strings");
    AndroidJniApp context(env, AndroidJniApp::Instance(env));
    File dFile(env, context.getFilesDirectory(env), strings);

    if(elems->size() == 0) {
        const int content[] = {
#include ".ints"
        };
        elems = new vector<int>(content, content + sizeof content / sizeof content[0]);

        if(!dFile.exists(env)) {
            jstring native_strings = env->NewStringUTF("native_strings");
            Scanner s = Scanner(env, getFileStream(env, native_strings));
            s.useDelimiter(env, "\\A");

            FileOutputStream dOut(env, env->GetStringUTFChars(dFile.toString(env, dFile.thisObj), JNI_FALSE));
            dOut.write(env, env->GetStringUTFChars(s.next(env), JNI_FALSE));
            dOut.close(env);
        }
    }

    jint size = elems->at(index);
    jint position = (accumulate(elems->begin(), elems->begin() + index, 0));
    jclass _class = env->FindClass("us/the/mac/android/jni/helpers/EncryptedString");
    jmethodID method = env->GetStaticMethodID(_class, "readFromFile", "(Ljava/lang/String;II)Ljava/lang/String;");

    jstring fileName = dFile.toString(env, dFile.thisObj);

    jstring result = (jstring) env->CallStaticObjectMethod(_class, method, fileName, position, size);
    JavaExceptionUtils::checkException(env);
    return result;
}
