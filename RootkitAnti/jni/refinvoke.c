#include <string.h>
#include <jni.h>

#include "refinvoke.h"
#include "debug.h"

jobject invokeObjectMethod (JNIEnv *env, const char* classname, const char* methodname, const char* methodsig, jobject obj, ...)
{
	jclass clazz = (*env)->FindClass(env, classname);
	if(clazz == NULL)
	{
		EGIS_ERROR("Class %s not found", classname);
		return NULL;
	}
	jmethodID method = (*env)->GetMethodID(env, clazz, methodname, methodsig);
	if(method == NULL)
	{
		EGIS_ERROR("Method %s not found", methodname);
		return NULL;
	}
	va_list args;
	va_start(args, obj);
	jobject result = (*env)->CallObjectMethodV(env, obj, method, args);
	va_end(args);
	if(result == NULL)
	{
		EGIS_ERROR("Method %s call failure", methodname);
		return NULL;
	}
	if((*env)->ExceptionCheck(env))
	{
		(*env)->ExceptionDescribe(env);  
        (*env)->ExceptionClear(env);  
	}
    (*env)->DeleteLocalRef(env, clazz);
	return result;
}

jint invokeIntMethod (JNIEnv *env, const char* classname, const char* methodname, const char* methodsig, jobject obj, ...)
{
	jclass clazz = (*env)->FindClass(env, classname);
	if(clazz == NULL)
	{
		EGIS_ERROR("Class %s not found", classname);
		return NULL;
	}
	jmethodID method = (*env)->GetMethodID(env, clazz, methodname, methodsig);
	if(method == NULL)
	{
		EGIS_ERROR("Method %s not found", methodname);
		return NULL;
	}
	va_list args;
	va_start(args, obj);
	jobject result = (*env)->CallIntMethodV(env, obj, method, args);
	va_end(args);
	if(result == NULL)
	{
		EGIS_ERROR("Method %s call failure", methodname);
		return NULL;
	}
	if((*env)->ExceptionCheck(env))
	{
		(*env)->ExceptionDescribe(env);  
        (*env)->ExceptionClear(env);  
	}
    (*env)->DeleteLocalRef(env, clazz);
	return result;
}

jboolean invokeBooleanMethod (JNIEnv *env, const char* classname, const char* methodname, const char* methodsig, jobject obj, ...)
{
	jclass clazz = (*env)->FindClass(env, classname);
	if(clazz == NULL)
	{
		EGIS_ERROR("Class %s not found", classname);
		return NULL;
	}
	jmethodID method = (*env)->GetMethodID(env, clazz, methodname, methodsig);
	if(method == NULL)
	{
		EGIS_ERROR("Method %s not found", methodname);
		return NULL;
	}
	va_list args;
	va_start(args, obj);
	jboolean result = (*env)->CallBooleanMethodV(env, obj, method, args);
	va_end(args);
	if(result == NULL)
	{
		EGIS_ERROR("Method %s call failure", methodname);
		return NULL;
	}
	if((*env)->ExceptionCheck(env))
	{
		(*env)->ExceptionDescribe(env);  
        (*env)->ExceptionClear(env);  
	}		
    (*env)->DeleteLocalRef(env, clazz);
	return result;
}

void invokeVoidMethod (JNIEnv *env, const char* classname, const char* methodname, const char* methodsig, jobject obj, ...)
{
	jclass clazz = (*env)->FindClass(env, classname);
	if(clazz == NULL)
	{
		EGIS_ERROR("Class %s not found", classname);
		return ;
	}
	jmethodID method = (*env)->GetMethodID(env, clazz, methodname, methodsig);
	if(method == NULL)
	{
		EGIS_ERROR("Method %s not found", methodname);
		return ;
	}
	va_list args;
	va_start(args, obj);
	(*env)->CallVoidMethodV(env, obj, method, args);
	va_end(args);
	if((*env)->ExceptionCheck(env))
	{
		(*env)->ExceptionDescribe(env);  
        (*env)->ExceptionClear(env);  
	}
    (*env)->DeleteLocalRef(env, clazz);
}

jint invokeStaticIntMethod (JNIEnv *env, const char* classname, const char* methodname, const char* methodsig, ...)
{
	jclass clazz = (*env)->FindClass(env, classname);
	if(clazz == NULL)
	{
		EGIS_ERROR("Class %s not found", classname);
		return NULL;
	}
	jmethodID method = (*env)->GetStaticMethodID(env, clazz, methodname, methodsig);
	if(method == NULL)
	{
		EGIS_ERROR("Method %s not found", methodname);
		return NULL;
	}
	va_list args;
	va_start(args, methodsig);
	jint result = (*env)->CallStaticIntMethodV(env, clazz, method, args);
	va_end(args);
	if(result == NULL)
	{
		EGIS_ERROR("Static method %s call failure", methodname);
		return NULL;
	}
	if((*env)->ExceptionCheck(env))
	{
		(*env)->ExceptionDescribe(env);  
        (*env)->ExceptionClear(env);  
	}		
	(*env)->DeleteLocalRef(env, clazz);
	return result;
}

jobject invokeStaticObjectMethod (JNIEnv *env, const char* classname, const char* methodname, const char* methodsig, ...)
{
	jclass clazz = (*env)->FindClass(env, classname);
	if(clazz == NULL)
	{
		EGIS_ERROR("Class %s not found", classname);
		return NULL;
	}

	jmethodID method = (*env)->GetStaticMethodID(env, clazz, methodname, methodsig);
	if(method == NULL)
	{
		EGIS_ERROR("Method %s not found", methodname);
		return NULL;
	}
	va_list args;
	va_start(args, methodsig);
	jobject result = (*env)->CallStaticObjectMethodV(env, clazz, method, args);
	va_end(args);
	if(result == NULL)
	{
		EGIS_ERROR("Static Method %s call failure", methodname);
		return NULL;
	}
	if((*env)->ExceptionCheck(env))
	{
		(*env)->ExceptionDescribe(env);  
        (*env)->ExceptionClear(env);  
	}	
	(*env)->DeleteLocalRef(env, clazz);
	return result;
}


jobject invokeSuperObjectMethod (JNIEnv *env, const char* classname, const char* methodname, const char* methodsig, jobject obj, ...)
{
	jclass clazz = (*env)->FindClass(env, classname);
	if(clazz == NULL)
	{
		EGIS_ERROR("Class %s not found", classname);
		return NULL;
	}
	jclass superclazz = (*env)->GetSuperclass(env, clazz);
	if(superclazz == NULL)
	{
		EGIS_ERROR("SuperClass %s not found", classname);
		return NULL;
	}
	jmethodID method = (*env)->GetMethodID(env, superclazz, methodname, methodsig);
	if(method == NULL)
	{
		EGIS_ERROR("Method %s not found", methodname);
		return NULL;
	}
	va_list args;
	va_start(args, obj);
	jobject result = (*env)->CallNonvirtualObjectMethodV(env, obj, superclazz, method, args);
	va_end(args);
	if(result == NULL)
	{
		EGIS_ERROR("Super Method %s call failure", methodname);
		return NULL;
	}
	if((*env)->ExceptionCheck(env))
	{
		(*env)->ExceptionDescribe(env);  
        (*env)->ExceptionClear(env);  
	}	
    (*env)->DeleteLocalRef(env, clazz);
	return result;
}

void invokeSuperVoidMethod (JNIEnv *env, const char* classname, const char* methodname, const char* methodsig, jobject obj, ...)
{
	jclass clazz = (*env)->FindClass(env, classname);
	if(clazz == NULL)
	{
		EGIS_ERROR("Class %s not found", classname);
		return ;
	}
	jclass superclazz = (*env)->GetSuperclass(env, clazz);
	if(superclazz == NULL)
	{
		EGIS_ERROR("SuperClass %s not found", classname);
		return ;
	}
	jmethodID method = (*env)->GetMethodID(env, superclazz, methodname, methodsig);
	if(method == NULL)
	{
		EGIS_ERROR("Method %s not found", methodname);
		return ;
	}
	va_list args;
	va_start(args, obj);
	(*env)->CallNonvirtualVoidMethodV(env, obj, superclazz, method, args);
	va_end(args);
	if((*env)->ExceptionCheck(env))
	{
		(*env)->ExceptionDescribe(env);  
        (*env)->ExceptionClear(env);  
	}
    (*env)->DeleteLocalRef(env, clazz);
}

jobject getOjbectField (JNIEnv *env, const char* classname, jobject obj, const char* filedname, const char* filedtype)
{
	jclass clazz = (*env)->FindClass(env, classname);
	if(clazz == NULL)
	{
		EGIS_ERROR("Class %s not found", classname);
		return NULL;
	}
	jfieldID field = (*env)->GetFieldID(env, clazz, filedname, filedtype);
	if(field == NULL)
	{
		EGIS_ERROR("Field %s %s not found", filedtype, filedname);
		return NULL;

	}
	jobject result = (*env)->GetObjectField(env, obj, field);
	if(result == NULL)
	{
		EGIS_ERROR("Field %s get failure", filedname);
		return NULL;	
	}
	if((*env)->ExceptionCheck(env))
	{
		(*env)->ExceptionDescribe(env);  
        (*env)->ExceptionClear(env);  
	}	
    (*env)->DeleteLocalRef(env, clazz);
	return result;
}

jint getIntField (JNIEnv *env, const char* classname, jobject obj, const char* filedname, const char* filedtype)
{
	jclass clazz = (*env)->FindClass(env, classname);
	if(clazz == NULL)
	{
		EGIS_ERROR("Class %s not found", classname);
		return NULL;	
	}
	jfieldID field = (*env)->GetFieldID(env, clazz, filedname, filedtype);
	if(field == NULL)
	{
		EGIS_ERROR("Field %s %s not found", filedtype, filedname);
		return NULL;
	}
	jint result = (*env)->GetIntField(env, obj, field);
	if(result == NULL)
	{
		EGIS_ERROR("Field %s get failure", filedname);
		return NULL;	
	}
	if((*env)->ExceptionCheck(env))
	{
		(*env)->ExceptionDescribe(env);  
        (*env)->ExceptionClear(env);  
	}
    (*env)->DeleteLocalRef(env, clazz);
	return result;
}

jobject getStaticOjbectField (JNIEnv *env, const char* classname, const char* filedname, const char* filedtype)
{
	jclass clazz = (*env)->FindClass(env, classname);
	if(clazz == NULL)
	{
		EGIS_ERROR("Class %s not found", classname);
		return NULL;	
	}
	jfieldID field = (*env)->GetStaticFieldID(env, clazz, filedname, filedtype);
	if(field == NULL)
	{
		EGIS_ERROR("Field %s %s not found", filedtype, filedname);
		return NULL;
	}
	jobject result = (*env)->GetStaticObjectField(env, clazz, field);
	if(result == NULL)
	{
		EGIS_ERROR("Field %s get failure", filedname);
		return NULL;	
	} 
	if((*env)->ExceptionCheck(env))
	{
		(*env)->ExceptionDescribe(env);  
        (*env)->ExceptionClear(env);  
	}   
    (*env)->DeleteLocalRef(env, clazz);

	return result;
}

jobject getSuperOjbectField (JNIEnv *env, const char* classname, jobject obj, const char* filedname, const char* filedtype)
{
	jclass clazz = (*env)->FindClass(env, classname);
	if(clazz == NULL)
	{
		EGIS_ERROR("Class %s not found", classname);
		return NULL;	
	}
	jclass superclazz = (*env)->GetSuperclass(env, clazz);
	if(superclazz == NULL)
	{
		EGIS_ERROR("SuperClass %s not found", classname);
		return NULL;	
	}
	jfieldID field = (*env)->GetFieldID(env, superclazz, filedname, filedtype);
	if(field == NULL)
	{
		EGIS_ERROR("Field %s %s not found", filedtype, filedname);
		return NULL;
	}
	jobject result = (*env)->GetObjectField(env, obj, field);
	if(result == NULL)
	{
		EGIS_ERROR("Field %s get failure", filedname);
		return NULL;	
	} 
	if((*env)->ExceptionCheck(env))
	{
		(*env)->ExceptionDescribe(env);  
        (*env)->ExceptionClear(env);  
	}   
	return result;
}

void setObjectField (JNIEnv *env, const char* classname, jobject obj, const char* filedname, const char* filedtype, jobject fieldvalue)
{
	jclass clazz = (*env)->FindClass(env, classname);
	if(clazz == NULL)
	{
		EGIS_ERROR("Class %s not found", classname);
		return ;
	}
	jfieldID field = (*env)->GetFieldID(env, clazz, filedname, filedtype);
	if(field == NULL)
	{
		EGIS_ERROR("Field %s %s not found", filedtype, filedname);
		return ;
	}
	(*env)->SetObjectField(env, obj, field, fieldvalue);
	if((*env)->ExceptionCheck(env))
	{
		(*env)->ExceptionDescribe(env);  
        (*env)->ExceptionClear(env);  
	}
}

void setStaticOjbectField (JNIEnv *env, const char* classname, const char* filedname, const char* filedtype, jobject fieldvalue)
{
	jclass clazz = (*env)->FindClass(env, classname);
	if(clazz == NULL)
	{
		EGIS_ERROR("Class %s not found", classname);
		return ;
	}
	jfieldID field = (*env)->GetStaticFieldID(env, clazz, filedname, filedtype);
	if(field == NULL)
	{
		EGIS_ERROR("Field %s %s not found", filedtype, filedname);
		return ;
	}
	(*env)->SetStaticObjectField(env, clazz, field, fieldvalue);
	if((*env)->ExceptionCheck(env))
	{
		(*env)->ExceptionDescribe(env);  
        (*env)->ExceptionClear(env);  
	}
}

void setSuperOjbectField (JNIEnv *env, const char* classname, jobject obj, const char* filedname, const char* filedtype, jobject fieldvalue)
{
	jclass clazz = (*env)->FindClass(env, classname);
	if(clazz == NULL)
	{
		EGIS_ERROR("Class %s not found", classname);
		return ;
	}
	jclass superclazz = (*env)->GetSuperclass(env, clazz);
	if(superclazz == NULL)
	{
		EGIS_ERROR("SuperClass %s not found", classname);
		return ;	
	}
	jfieldID field = (*env)->GetFieldID(env, superclazz, filedname, filedtype);
	if(field == NULL)
	{
		EGIS_ERROR("Field %s %s not found", filedtype, filedname);
		return ;
	}
	(*env)->SetObjectField(env, obj, field, fieldvalue);
	if((*env)->ExceptionCheck(env))
	{
		(*env)->ExceptionDescribe(env);  
        (*env)->ExceptionClear(env);  
	}
}
