#include <iostream>
#include <pthread.h>
#include <jnihook.h>

JavaVM *jvm;
jclass PlayerClass;
jmethodID AttackMethod;

jvalue hkPlayerAttack(jvalue *args, size_t nargs, void *thread, void *arg)
{
	JNIEnv *jni;
	
	std::cout << "[DH] hkPlayerAttack called!" << std::endl;
	std::cout << "[DH] Number of args: " << nargs << std::endl;
	std::cout << "[DH] Args: " << std::endl;
	std::cout << "[DH]  - thisptr: " << (void *)(args[0].l) << std::endl;
	std::cout << "[DH]  - level: " << (void *)(args[1].l) << std::endl;

	jvm->GetEnv((void **)&jni, JNI_VERSION_1_6);
	std::cout << "[DH] JNI: " << jni << std::endl;

	std::cout << "[DH] Calling original Player::Attack..." << std::endl;

	jni->CallVoidMethod((jobject)&args[0].l, AttackMethod, (jobject)&args[1].l);

	std::cout << "[DH] Called original Player::Attack" << std::endl;
	
	return jvalue { 0 };
}

static void setup(JNIEnv *jni)
{
	PlayerClass = jni->FindClass("com/interrupt/dungeoneer/entities/Player");
	if (!PlayerClass) {
		std::cout << "[DH] Failed to find Player class" << std::endl;
		return;
	}
	std::cout << "[DH] Player class: " << PlayerClass << std::endl;

	AttackMethod = jni->GetMethodID(PlayerClass, "Attack", "(Lcom/interrupt/dungeoneer/game/Level;)V");
	std::cout << "[DH] Attack method: " << AttackMethod << std::endl;

	JNIHook_Init(jvm);
	JNIHook_Attach(AttackMethod, hkPlayerAttack, NULL);

	std::cout << "[DH] Delver Hook set up successfully" << std::endl;
}

static void *main_thread(void *arg)
{
	JNIEnv *jni;

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
