#include <iostream>
#include <pthread.h>
#include <jnihook.h>

JavaVM *jvm;
jclass PlayerClass;

static void setup(JNIEnv *jni)
{
	PlayerClass = jni->FindClass("com/interrupt/dungeoneer/entities/Player");
	if (!PlayerClass) {
		std::cout << "[DH] Failed to find Player class" << std::endl;
		return;
	}
	std::cout << "[DH] Player class: " << PlayerClass << std::endl;

	JNIHook_Init(jvm);

	std::cout << "[DH] Delver Hook set up successfully" << std::endl;
}

static void *main_thread(void *arg)
{
	JNIEnv *jni;

	std::system("zenity --info");

	freopen("/tmp/delverhook.log", "w", stdout);

	std::cout << "[DH] Library loaded!" << std::endl;

	if (JNI_GetCreatedJavaVMs(&jvm, 1, NULL) != JNI_OK) {
		std::cout << "[DH] Failed to retrieve JavaVM pointer" << std::endl;
		return NULL;
	}

	std::cout << "[DH] JavaVM: " << jvm << std::endl;

	if (jvm->AttachCurrentThread((void **)&jni, NULL) != JNI_OK) {
		std::cout << "[DH] Failed to retrieve JNI environment" << std::endl;
		return NULL;
	}

	std::cout << "[DH] JNIEnv: " << jni << std::endl;

	setup(jni);

	return arg;
}

void __attribute__((constructor))
dl_entry()
{
	pthread_t th;
	pthread_create(&th, NULL, main_thread, NULL);
}
