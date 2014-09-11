#ifndef _REFINVOKE_H_
#define _REFINVOKE_H_

extern jobject invokeObjectMethod (JNIEnv *env, const char* classname, const char* methodname, const char* methodsig, jobject obj, ...);

extern jint invokeIntMethod (JNIEnv *env, const char* classname, const char* methodname, const char* methodsig, jobject obj, ...);

extern jboolean invokeBooleanMethod (JNIEnv *env, const char* classname, const char* methodname, const char* methodsig, jobject obj, ...);

extern void invokeVoidMethod(JNIEnv *env, const char* classname, const char* methodname, const char* methodsig, jobject obj, ...);

extern jint invokeStaticIntMethod (JNIEnv *env, const char* classname, const char* methodname, const char* methodsig, ...);

extern jobject invokeStaticObjectMethod (JNIEnv *env, const char* classname, const char* methodname, const char* methodsig, ...);

extern jobject invokeSuperObjectMethod (JNIEnv *env, const char* classname, const char* methodname, const char* methodsig, jobject obj, ...);

extern jobject getOjbectField (JNIEnv *env, const char* classname, jobject obj, const char* filedname, const char* filedtype);

extern jint getIntField (JNIEnv *env, const char* classname, jobject obj, const char* filedname, const char* filedtype);

extern jobject getStaticOjbectField (JNIEnv *env, const char* classname, const char* filedname, const char* filedtype);

extern jobject getSuperOjbectField (JNIEnv *env, const char* classname, jobject obj, const char* filedname, const char* filedtype);

extern void setObjectField (JNIEnv *env, const char* classname, jobject obj, const char* filedname, const char* filedtype, jobject fieldvalue);

extern void setStaticOjbectField (JNIEnv *env, const char* classname, const char* filedname, const char* filedtype, jobject fieldvalue);

extern void setSuperOjbectField (JNIEnv *env, const char* classname, jobject obj, const char* filedname, const char* filedtype, jobject fieldvalue);

#endif  

/*_REFINVOKE_H_*/
