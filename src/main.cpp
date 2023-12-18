#include <iostream>
#include <pthread.h>
#include <jnihook.h>

JavaVM *jvm;
jclass PlayerClass;
jmethodID AttackMethod;
jmethodID takeDamageMethod;
jmethodID getWalkSpeedMethod;
jmethodID getAttackSpeedStatBoostMethod;

jvalue hkPlayerAttack(JNIEnv *jni, jmethodID callableMethod, jvalue *args, size_t nargs, void *arg)
{
	std::cout << "[DH] hkPlayerAttack called!" << std::endl;
	std::cout << "[DH] Number of args: " << nargs << std::endl;
	std::cout << "[DH] Args: " << std::endl;
	std::cout << "[DH]  - thisptr: " << (void *)(args[0].l) << std::endl;
	std::cout << "[DH]  - level: " << (void *)(args[1].l) << std::endl;

	std::cout << "[DH] JNI: " << jni << std::endl;

	std::cout << "[DH] Calling original Player::Attack..." << std::endl;

	jni->CallVoidMethod((jobject)&args[0].l, callableMethod, (jobject)&args[1].l);

	std::cout << "[DH] Called original Player::Attack" << std::endl;
	
	return jvalue { 0 };
}


jvalue hkTakeDamage(JNIEnv *jni, jmethodID callableMethod, jvalue *args, size_t nargs, void *arg)
{
	std::cout << "[DH] hkTakeDamage called!" << std::endl;
	std::cout << "[DH] Number of args: " << nargs << std::endl;
	std::cout << "[DH] Args: " << std::endl;
	std::cout << "[DH]  - thisptr: " << (void *)(args[0].l) << std::endl;
	std::cout << "[DH]  - damage: " << args[1].i << std::endl;
	std::cout << "[DH]  - damageType: " << (void *)(args[2].l) << std::endl;
	std::cout << "[DH]  - instigator: " << (void *)(args[3].l) << std::endl;

	return jvalue { 0 };
}

jvalue hkGetWalkSpeed(JNIEnv *jni, jmethodID callableMethod, jvalue *args, size_t nargs, void *arg)
{
	std::cout << "[DH] hkGetWalkSpeed called!" << std::endl;
	std::cout << "[DH] Number of args: " << nargs << std::endl;
	std::cout << "[DH] Args: " << std::endl;
	std::cout << "[DH]  - thisptr: " << (void *)(args[0].l) << std::endl;

	return jvalue { .f = 0.75f }; // Default speed is lower than this
}

jvalue hkGetAttackSpeedStatBoost(JNIEnv *jni, jmethodID callableMethod, jvalue *args, size_t nargs, void *arg)
{
	std::cout << "[DH] hkGetAttackSpeedStatBoost called!" << std::endl;
	std::cout << "[DH] Number of args: " << nargs << std::endl;
	std::cout << "[DH] Args: " << std::endl;
	std::cout << "[DH]  - thisptr: " << (void *)(args[0].l) << std::endl;

	return jvalue { .f = 10.0f };
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

	takeDamageMethod = jni->GetMethodID(PlayerClass, "takeDamage", "(ILcom/interrupt/dungeoneer/entities/items/Weapon$DamageType;Lcom/interrupt/dungeoneer/entities/Entity;)I");
	std::cout << "[DH] takeDamage method: " << takeDamageMethod << std::endl;

	getWalkSpeedMethod = jni->GetMethodID(PlayerClass, "getWalkSpeed", "()F");
	std::cout << "[DH] getWalkSpeed method: " << getWalkSpeedMethod << std::endl;

	getAttackSpeedStatBoostMethod = jni->GetMethodID(PlayerClass, "getAttackSpeedStatBoost", "()F");
	std::cout << "[DH] getAttackSpeedStatBoost method: " << getAttackSpeedStatBoostMethod << std::endl;

	JNIHook_Init(jvm);
	jint hkresult;

	hkresult = JNIHook_Attach(AttackMethod, hkPlayerAttack, NULL);
	std::cout << "[DH] Player::Attack Hook Result: " << hkresult << std::endl;

	hkresult = JNIHook_Attach(takeDamageMethod, hkTakeDamage, NULL);
	std::cout << "[DH] Player::takeDamage Hook Result: " << hkresult << std::endl;

	hkresult = JNIHook_Attach(getWalkSpeedMethod, hkGetWalkSpeed, NULL);
	std::cout << "[DH] Player::getWalkSpeed Hook Result: " << hkresult << std::endl;

	hkresult = JNIHook_Attach(getAttackSpeedStatBoostMethod, hkGetAttackSpeedStatBoost, NULL);
	std::cout << "[DH] Player::getWalkSpeed Hook Result: " << hkresult << std::endl;

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
