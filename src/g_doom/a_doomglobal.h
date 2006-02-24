#ifndef __A_DOOMGLOBAL_H__
#define __A_DOOMGLOBAL_H__

#include "dobject.h"
#include "info.h"
#include "d_player.h"

class ABossBrain : public AActor
{
	DECLARE_ACTOR (ABossBrain, AActor)
};

class AExplosiveBarrel : public AActor
{
	DECLARE_ACTOR (AExplosiveBarrel, AActor)
public:
	const char *GetObituary ();
};

class ABulletPuff : public AActor
{
	DECLARE_ACTOR (ABulletPuff, AActor)
public:
	void BeginPlay ();
};

class ARocket : public AActor
{
	DECLARE_ACTOR (ARocket, AActor)
public:
	void BeginPlay ();
	const char *GetObituary ();
};

class AArchvile : public AActor
{
	DECLARE_ACTOR (AArchvile, AActor)
public:
	const char *GetObituary ();
};

class ALostSoul : public AActor
{
	DECLARE_ACTOR (ALostSoul, AActor)
public:
	const char *GetObituary ();
};

class APlasmaBall : public AActor
{
	DECLARE_ACTOR (APlasmaBall, AActor)
	const char *GetObituary ();
};

class ABFGBall : public AActor
{
	DECLARE_ACTOR (ABFGBall, AActor)
public:
	const char *GetObituary ();
};

class AScriptedMarine : public AActor
{
	DECLARE_ACTOR (AScriptedMarine, AActor)
public:
	enum EMarineWeapon
	{
		WEAPON_Dummy,
		WEAPON_Fist,
		WEAPON_BerserkFist,
		WEAPON_Chainsaw,
		WEAPON_Pistol,
		WEAPON_Shotgun,
		WEAPON_SuperShotgun,
		WEAPON_Chaingun,
		WEAPON_RocketLauncher,
		WEAPON_PlasmaRifle,
		WEAPON_Railgun,
		WEAPON_BFG
	};

	void Activate (AActor *activator);
	void Deactivate (AActor *activator);
	void BeginPlay ();
	void Tick ();
	void SetWeapon (EMarineWeapon);
	void SetSprite (const TypeInfo *source);
	void Serialize (FArchive &arc);

protected:
	int SpriteOverride;
};

class ADoomPlayer : public APlayerPawn
{
	DECLARE_ACTOR (ADoomPlayer, APlayerPawn)
public:
	void GiveDefaultInventory ();
};

#endif //__A_DOOMGLOBAL_H__
