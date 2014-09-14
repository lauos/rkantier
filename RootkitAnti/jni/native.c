#include <stdio.h>
#include <stdlib.h>
#include <jni.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>             // 提供open()函数
#include <dirent.h>            // 提供目录流操作函数
#include <string.h>
#include <sys/stat.h>        // 提供属性操作函数
#include <errno.h>


#include "debug.h"
#include "refinvoke.h"
#include "modify_mod.h"
#include "reslove_mod.h"

#define NELEM(x) ((int)(sizeof(x) / sizeof((x)[0])))


int is_module(char *name)
{
	char *p;
	int len = strlen(name);
	int namelen = 0;
	if((p = strstr(name, ".ko")) != NULL)
	{
		namelen = (int)(p - name);
		if((len - namelen) == 3)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	if((p = strstr(name, ".o")) != NULL)
	{
		namelen = (int)(p - name);
		if((len - namelen) == 2)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

int read_proc_info(char* dir, char* buf)
{
	int fp;
	char path[256];

	sprintf(path, "%s/%s", "/proc/anti-rk", dir);
	EGIS_DEBUG("%s to read", path);

	fp = open(path, O_RDONLY);
	if(fp == -1)
	{
		EGIS_ERROR("open %s failed!", path);
		return -1;
	}

	if(read(fp, buf, 1024) == -1)
	{
		EGIS_ERROR("read %s failed! : %s", path);
		close(fp);
		return -1;
	}
	close(fp);
	return 0;
}

int write_proc_info(char* dir, char* buf)
{
	int fp;
	char path[256];
	sprintf(path, "%s/%s", "/proc/anti-rk", dir);
	EGIS_DEBUG("%s to write", path);

	fp = open(path, O_RDWR);
	if(fp == -1)
	{
		EGIS_ERROR("open %s failed!", path);
		return -1;
	}

	if(write(fp, buf, strlen(buf)) == -1)
	{
		close(fp);
		return -1;
	}
	close(fp);
	return 0;
}

const char* scan_dir(char *dir)   // 定义目录扫描函数
{
    DIR *dp;                      // 定义子目录流指针
    struct dirent *entry;         // 定义dirent结构指针保存后续目录
    struct stat statbuf;          // 定义statbuf结构保存文件属性

    char* mod_path = (char *)calloc(1, 256);

    if((dp = opendir(dir)) == NULL) // 打开目录，获取子目录流指针，判断操作是否成功
    {
        //EGIS_ERROR("Can't open dir!");
        return;
    }
    chdir(dir);                     // 切换到当前目录
    while((entry = readdir(dp)) != NULL)  // 获取下一级目录信息，如果未否则循环
    {
        lstat(entry->d_name, &statbuf); // 获取下一级成员属性
        if(S_IFDIR & (statbuf.st_mode))    // 判断下一级成员是否是目录
        {
            if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
              continue;

            //EGIS_DEBUG("%s",entry->d_name);  // 输出目录名称
            //scan_dir(entry->d_name);              // 递归调用自身，扫描下一级目录的内容
        }
        else
        {
            if(is_module(entry->d_name))
            {
            	//EGIS_DEBUG("%s", entry->d_name);  // 输出属性不是目录的成员
            	sprintf(mod_path, "%s/%s", dir, entry->d_name);
            	return mod_path;
            }
        }
    }
    chdir("..");                                                  // 回到上级目录
    closedir(dp);                                                 // 关闭子目录流

    return NULL;
}

jboolean enableMod(JNIEnv* env, jclass clazz, jobject object)
{
	EGIS_DEBUG("In enableMod");

	//need to be free
	const char* mod_path = scan_dir("/system/lib/modules");
	EGIS_DEBUG("Mod path : %s", mod_path);

    jstring dataDir = (jstring)getOjbectField(env, "android/content/pm/ApplicationInfo", object, "dataDir", "Ljava/lang/String;");

    char* datadir = (char *)(*env)->GetStringUTFChars(env, dataDir, 0);

    EGIS_DEBUG("dataDir :%s", datadir);

    char rk_path[100];
    sprintf(rk_path, "%s%s%s", datadir, "/","antirk.ko");

    char true_magic[512];
    unsigned long true_checksum;

    load_and_reslove(mod_path, true_magic, &true_checksum);

    EGIS_DEBUG("true_magic : %s", true_magic);
    EGIS_DEBUG("true_checksum : %08x", true_checksum);
    EGIS_DEBUG("rk_path : %s", rk_path);

    load_and_modify(rk_path, true_magic, true_checksum);

	pid_t pid;
	int status;
	pid = fork();
	if(pid < 0)
	{
		EGIS_ERROR("Fork child process error!");
		exit(0);
	}else if(pid == 0)
	{
	    char cmd[100];
	    sprintf(cmd, "%s %s", "insmod", rk_path);

		execl("/system/bin/su", "/system/bin/su", "-c", cmd, NULL);
	}
	else
	{
		if (waitpid(pid, &status, 0) != pid)
		{
			EGIS_ERROR("waitpid failed");
			return 0;
		 }

		 if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
		 {
			 EGIS_ERROR("failed");
			 return 0;
		 }else
		 {
			 EGIS_DEBUG("success");
		 }
	}

	char result[1024];
	if(read_proc_info("enable", result) < 0)
	{
		return 0;
	}
	return 1;
}

jstring startDetection(JNIEnv* env, jclass clazz, jstring config)
{
	EGIS_DEBUG("In start");

	char buf[1024];
	char* config_str = (char *)(*env)->GetStringUTFChars(env, config, 0);

	sprintf(buf, "%s%s", "Detect:", config_str);
	EGIS_DEBUG("Config : %s", buf);

	if(write_proc_info("detection", buf) < 0)
	{
		EGIS_ERROR("Write config error");
		return 0;
	}

	do
	{
		if(read_proc_info("detection", buf) < 0)
		{
			EGIS_ERROR("Read result error: %s", buf);
			return 0;
		}
		if(strstr(buf, "Result:") != NULL)
		{
			EGIS_DEBUG("Got Result");
			break;
		}
	}while(1);

	return (*env)->NewGlobalRef(env, (*env)->NewStringUTF(env, buf));
}
jboolean resetSystem(JNIEnv* env, jclass clazz)
{
	EGIS_DEBUG("In resetSystem");
	return 1;
}

jboolean disableMod(JNIEnv* env, jclass clazz)
{
	EGIS_DEBUG("In disableMod");
	return 1;
}

static JNINativeMethod gMethods[] =
{
    {"enableMod", "(Landroid/content/pm/ApplicationInfo;)Z", (void *)enableMod},
    {"disableMod", "()Z", (void *)disableMod},
    {"startDetection", "(Ljava/lang/String;)Ljava/lang/String;", (void*)startDetection},
    {"resetSystem", "()Z", (void*)resetSystem}
};

/*
* Register several native methods for one class.
*/
static int registerNativeMethods(JNIEnv* env, const char* className, JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz = (*env)->FindClass(env, className);
    if (clazz == NULL)
    {
        return JNI_ERR;
    }

    if ((*env)->RegisterNatives(env, clazz, gMethods, numMethods) < 0)
    {
        return JNI_ERR;
    }

    return JNI_TRUE;
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;
	jint result = -1;

	EGIS_DEBUG("JNI_OnLoad");

	if (((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_6) != JNI_OK))
	{
		return JNI_ERR;
	}

    assert(env != NULL);

    if (!registerNativeMethods(env, "com/rootkit/anti/NativeFuncs", gMethods, NELEM(gMethods)))
    {
        return JNI_ERR;
    }

	result = JNI_VERSION_1_6;
	return result;
}
