#include <iostream>
#include <pthread.h>
#include <jnihook.h>

JavaVM *jvm;
jclass PlayerClass;
jmethodID AttackMethod;
jmethodID orig_AttackMethod;
jmethodID takeDamageMethod;
jmethodID orig_takeDamageMethod;
jmethodID getWalkSpeedMethod;
jmethodID orig_getWalkSpeedMethod;
jmethodID getAttackSpeedStatBoostMethod;
jmethodID orig_getAttackSpeedStatBoostMethod;

void hkPlayerAttack(JNIEnv *jni, jobject obj, jobject level)
{
	std::cout << "[DH] hkPlayerAttack called!" << std::endl;
	std::cout << "[DH] Args: " << std::endl;
	std::cout << "[DH]  - thisptr: " << (void *)(obj) << std::endl;
	std::cout << "[DH]  - level: " << (void *)(level) << std::endl;

	std::cout << "[DH] JNI: " << jni << std::endl;

	std::cout << "[DH] Calling original Player::Attack..." << std::endl;

	jni->CallNonvirtualVoidMethod(obj, PlayerClass, orig_AttackMethod, level);

	std::cout << "[DH] Called original Player::Attack" << std::endl;
}

jint hkTakeDamage(JNIEnv *jni, jobject obj, jint damage, jobject damageType, jobject instigator)
{
	std::cout << "[DH] hkTakeDamage called!" << std::endl;
	std::cout << "[DH] Args: " << std::endl;
	std::cout << "[DH]  - thisptr: " << (void *)obj << std::endl;
	std::cout << "[DH]  - damage: " << damage << std::endl;
	std::cout << "[DH]  - damageType obj: " << (void *)damageType << std::endl;
	std::cout << "[DH]  - instigator obj: " << (void *)instigator << std::endl;

	return 0;
}

jfloat hkGetWalkSpeed(JNIEnv *jni, jobject obj)
{
	std::cout << "[DH] hkGetWalkSpeed called!" << std::endl;
	std::cout << "[DH] Args: " << std::endl;
	std::cout << "[DH]  - thisptr: " << (void *)obj << std::endl;

	return 0.75f; // Default speed is lower than this
}

jfloat hkGetAttackSpeedStatBoost(JNIEnv *jni, jobject obj)
{
	std::cout << "[DH] hkGetAttackSpeedStatBoost called!" << std::endl;
	std::cout << "[DH] Args: " << std::endl;
	std::cout << "[DH]  - thisptr: " << (void *)obj << std::endl;

	return 10.0f;
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

	hkresult = JNIHook_Attach(AttackMethod, (void *)hkPlayerAttack, &orig_AttackMethod);
	std::cout << "[DH] Player::Attack Hook Result: " << hkresult << std::endl;

	hkresult = JNIHook_Attach(takeDamageMethod, (void *)hkTakeDamage, &orig_takeDamageMethod);
	std::cout << "[DH] Player::takeDamage Hook Result: " << hkresult << std::endl;

	hkresult = JNIHook_Attach(getWalkSpeedMethod, (void *)hkGetWalkSpeed, &orig_getWalkSpeedMethod);
	std::cout << "[DH] Player::getWalkSpeed Hook Result: " << hkresult << std::endl;

	hkresult = JNIHook_Attach(getAttackSpeedStatBoostMethod, (void *)hkGetAttackSpeedStatBoost, &orig_getAttackSpeedStatBoostMethod);
	std::cout << "[DH] Player::getWalkSpeed Hook Result: " << hkresult << std::endl;

	std::cout << "[DH] Delver Hook set up successfully" << std::endl;
}

static void *main_thread(void *arg)
{
	JNIEnv *jni;

	freopen("/tmp/delverhook.out", "w", stdout);
	freopen("/tmp/delverhook.err", "w", stderr);

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
