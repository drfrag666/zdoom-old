/*
** thingdef.cpp
**
** Code pointers for Actor definitions
**
**---------------------------------------------------------------------------
** Copyright 2002-2005 Christoph Oelckers
** Copyright 2004-2005 Randy Heit
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
** 4. When not used as part of ZDoom or a ZDoom derivative, this code will be
**    covered by the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or (at
**    your option) any later version.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
*/

#include "gi.h"
#include "actor.h"
#include "info.h"
#include "sc_man.h"
#include "tarray.h"
#include "w_wad.h"
#include "templates.h"
#include "r_defs.h"
#include "r_draw.h"
#include "a_pickups.h"
#include "s_sound.h"
#include "cmdlib.h"
#include "p_lnspec.h"
#include "p_enemy.h"
#include "a_action.h"
#include "decallib.h"
#include "m_random.h"
#include "autosegs.h"
#include "i_system.h"
#include "p_local.h"
#include "c_console.h"
#include "doomerrors.h"
#include "vectors.h"
#include "a_sharedglobal.h"
#include "a_doomglobal.h"


static FRandom pr_camissile ("CustomActorfire");
static FRandom pr_camelee ("CustomMelee");
static FRandom pr_cabullet ("CustomBullet");
static FRandom pr_cajump ("CustomJump");
static FRandom pr_cwbullet ("CustomWpBullet");
static FRandom pr_cwjump ("CustomWpJump");
static FRandom pr_cwpunch ("CustomWpPunch");
static FRandom pr_grenade ("ThrowGrenade");
static FRandom pr_crailgun ("CustomRailgun");
static FRandom pr_spawndebris ("SpawnDebris");
static FRandom pr_jiggle ("Jiggle");


int EvalExpressionI (int id, AActor *self) { return id; }
float EvalExpressionF (int id, AActor *self) { return id; }


// A truly awful hack to get to the state that called an action function
// without knowing whether it has been called from a weapon or actor.
FState * CallingState;

struct StateCallData
{
	FState * State;
	bool Result;
};

StateCallData StateCall;

//==========================================================================
//
// Executes the code pointers in a chain of states
// until there is no next state
//
//==========================================================================

bool CallStateChain(AActor * actor, FState * State)
{
	bool result = false;
	int counter=0;

	StateCall.State = State;
	while (StateCall.State !=NULL)
	{
		// Assume success. The code pointer will set this to false if necessary
		StateCall.Result = true;	
		CallingState = StateCall.State;
		StateCall.State->GetAction() (actor);

		// collect all the results. Even one successful call signifies overall success.
		result |= StateCall.Result;

		// Since there are no delays it is a good idea to check for infinite loops here!
		counter++;
		if (counter>=10000)	break;

		if (StateCall.State == CallingState) 
		{
			// Abort immediately if the state jumps to itself!
			if (StateCall.State == StateCall.State->GetNextState()) return false;
			
			// If both variables are still the same there was no jump
			// so we must advance to the next state.
			StateCall.State = StateCall.State->GetNextState();
		}
	}
	return result;
}

//==========================================================================
//
// Let's isolate all handling of CallingState in this one place 
// so that removing it later becomes easier
//
//==========================================================================
int CheckIndex(int paramsize, FState ** pcallstate)
{
	if (!(CallingState->Frame&SF_STATEPARAM)) return -1;

	unsigned int index = CallingState->GetMisc1_2();
	if (index > StateParameters.Size()-paramsize-2) return -1;
	if (pcallstate) *pcallstate=CallingState;
	return index+2;	// skip the misc parameters
}


//==========================================================================
//
// Simple flag changers
//
//==========================================================================
void A_SetSolid(AActor * self)
{
	self->flags |= MF_SOLID;
}

void A_UnsetSolid(AActor * self)
{
	self->flags &= ~MF_SOLID;
}

void A_SetFloat(AActor * self)
{
	self->flags |= MF_FLOAT;
}

void A_UnsetFloat(AActor * self)
{
	self->flags &= ~(MF_FLOAT|MF_INFLOAT);
}

//==========================================================================
//
// Customizable attack functions which use actor parameters.
// I think this is among the most requested stuff ever ;-)
//
//==========================================================================
static void DoAttack (AActor *self, bool domelee, bool domissile)
{
	int index=CheckIndex(4);

	if (index<0) return;
	if (self->target == NULL) return;

	int MeleeDamage=StateParameters[index];
	int MeleeSound=StateParameters[index+1];
	const char *MissileName=(const char *)StateParameters[index+2];
	fixed_t MissileHeight=StateParameters[index+3];

	A_FaceTarget (self);
	if (domelee && MeleeDamage>0 && self->CheckMeleeRange ())
	{
		int damage = pr_camelee.HitDice(MeleeDamage);
		if (MeleeSound) S_SoundID (self, CHAN_WEAPON, MeleeSound, 1, ATTN_NORM);
		P_DamageMobj (self->target, self, self, damage, MOD_HIT);
		P_TraceBleed (damage, self->target, self);
	}
	else if (domissile && MissileName)
	{
		const TypeInfo * ti=TypeInfo::FindType(MissileName);
		if (ti) 
		{
			// Although there is a P_SpawnMissileZ function its
			// aiming is much too bad to be of any use
			self->z+=MissileHeight-32*FRACUNIT;
			AActor * missile = P_SpawnMissile (self, self->target, ti);
			self->z-=MissileHeight-32*FRACUNIT;

			if (missile)
			{
				// automatic handling of seeker missiles
				if (missile->flags2&MF2_SEEKERMISSILE)
				{
					missile->tracer=self->target;
				}
				// set the health value so that the missile works properly
				if (missile->flags4&MF4_SPECTRAL)
				{
					missile->health=-2;
				}
			}
		}
	}
}

void A_MeleeAttack(AActor * self)
{
	DoAttack(self, true, false);
}

void A_MissileAttack(AActor * self)
{
	DoAttack(self, false, true);
}

void A_ComboAttack(AActor * self)
{
	DoAttack(self, true, true);
}

//==========================================================================
//
// Custom sound functions. These use misc1 and misc2 in the state structure
// This has been changed to use the parameter array instead of using the
// misc field directly so they can be used in weapon states
//
//==========================================================================
static void DoPlaySound(AActor * self, int channel)
{
	int index=CheckIndex(1);
	if (index<0) return;

	int soundid = StateParameters[index];
	S_SoundID (self, channel, soundid, 1, ATTN_NORM);
}

void A_PlaySound(AActor * self)
{
	DoPlaySound(self, CHAN_BODY);
}

void A_PlayWeaponSound(AActor * self)
{
	DoPlaySound(self, CHAN_WEAPON);
}

void A_StopSound(AActor * self)
{
	S_StopSound(self, CHAN_VOICE);
}

//==========================================================================
//
// Generic seeker missile function
//
//==========================================================================
void A_SeekerMissile(AActor * self)
{
	int index=CheckIndex(2);
	if (index<0) return;

	P_SeekerMissile(self,
		clamp<int>(EvalExpressionI (StateParameters[index], self), 0, 90) * ANGLE_1,
		clamp<int>(EvalExpressionI (StateParameters[index+1], self), 0, 90) * ANGLE_1);
}

//==========================================================================
//
// Hitscan attack with a customizable amount of bullets (specified in damage)
//
//==========================================================================
void A_BulletAttack (AActor *self)
{
	int i;
	int bangle;
	int slope;
		
	if (!self->target) return;

	A_FaceTarget (self);
	bangle = self->angle;

	slope = P_AimLineAttack (self, bangle, MISSILERANGE);

	S_SoundID (self, CHAN_WEAPON, self->AttackSound, 1, ATTN_NORM);
	for (i=0 ; i<self->damage ; i++)
    {
		int angle = bangle + (pr_cabullet.Random2() << 20);
		int damage = ((pr_cabullet()%5)+1)*3;
		P_LineAttack(self, angle, MISSILERANGE, slope, damage,
			GetDefaultByType(RUNTIME_CLASS(ABulletPuff))->DamageType, RUNTIME_CLASS(ABulletPuff));
    }
}

//==========================================================================
//
// Do the state jump
//
//==========================================================================
static void DoJump(AActor * self, FState * CallingState, int offset)
{
	if (!self->player) 
	{
		self->SetState (CallingState + offset);
	}
	else if (CallingState == self->player->psprites[ps_weapon].state)
	{
		P_SetPsprite(self->player, ps_weapon, CallingState + offset);
	}
	else if (CallingState == self->player->psprites[ps_flash].state)
	{
		P_SetPsprite(self->player, ps_flash, CallingState + offset);
	}
	else if (CallingState == StateCall.State)
	{
		StateCall.State += offset;
	}
	StateCall.Result=false;	// Jumps should never set the result for inventory state chains!
}
//==========================================================================
//
// State jump function
//
//==========================================================================
void A_Jump(AActor * self)
{
	FState * CallingState;
	int index=CheckIndex(2, &CallingState);

	if (index>=0 && pr_cajump() < clamp<int>(EvalExpressionI (StateParameters[index], self), 0, 255))
		DoJump(self, CallingState, StateParameters[index+1]);
}

//==========================================================================
//
// State jump function
//
//==========================================================================
void A_JumpIfHealthLower(AActor * self)
{
	FState * CallingState;
	int index=CheckIndex(2, &CallingState);

	if (index>=0 && self->health < EvalExpressionI (StateParameters[index], self))
		DoJump(self, CallingState, StateParameters[index+1]);
}

//==========================================================================
//
// State jump function
//
//==========================================================================
void A_JumpIfCloser(AActor * self)
{
	FState * CallingState;
	int index=CheckIndex(2, &CallingState);
	AActor * target;

	if (!self->player)
	{
		target=self->target;
	}
	else
	{				
		// Does the player aim at something that can be shot?
		P_BulletSlope(self);
		target = linetarget;
	}

	// No target - no jump
	if (target==NULL) return;

	fixed_t dist = EvalExpressionF (StateParameters[index], self) * FRACUNIT;
	if (index>0 && P_AproxDistance(self->x-self->target->x, self->y-self->target->y) < dist)
		DoJump(self, CallingState, StateParameters[index+1]);
}

//==========================================================================
//
// State jump function
//
//==========================================================================
void A_JumpIfInventory(AActor * self)
{
	FState * CallingState;
	int index=CheckIndex(3, &CallingState);
	if (index<0) return;

	const char * ItemType=(const char *)StateParameters[index];
	int ItemAmount = EvalExpressionI (StateParameters[index+1], self);
	int JumpOffset = StateParameters[index+2];
	const TypeInfo * Type=TypeInfo::FindType(ItemType);

	if (!Type) return;

	AInventory * Item=self->FindInventory(Type);

	if (Item)
	{
		if (ItemAmount>0 && Item->Amount>=ItemAmount) DoJump(self, CallingState, JumpOffset);
		else if (Item->Amount>=Item->MaxAmount) DoJump(self, CallingState, JumpOffset);
	}
}


//==========================================================================
//
// Parameterized version of A_Explode
//
//==========================================================================

void A_ExplodeParms (AActor *self)
{
	int damage = 128;
	int distance = 128;
	bool hurtSource = true;

	int index=CheckIndex(3);
	if (index>=0) 
	{
		if (StateParameters[index] != 0)
		{
			damage = StateParameters[index];
		}
		if (StateParameters[index+1] != 0)
		{
			distance = StateParameters[index+1];
		}
		hurtSource = !!StateParameters[index+2];
	}

	P_RadiusAttack (self, self->target, damage, distance, self->DamageType, hurtSource);
	if (self->z <= self->floorz + (distance<<FRACBITS))
	{
		P_HitFloor (self);
	}
}


//==========================================================================
//
// Execute a line special / script
//
//==========================================================================
void A_CallSpecial(AActor * self)
{
	int index=CheckIndex(6);
	if (index<0) return;

	StateCall.Result = !!LineSpecials[StateParameters[index]](NULL, self, false,
		EvalExpressionI (StateParameters[index+1], self),
		EvalExpressionI (StateParameters[index+2], self),
		EvalExpressionI (StateParameters[index+3], self),
		EvalExpressionI (StateParameters[index+4], self),
		EvalExpressionI (StateParameters[index+5], self));
}

//==========================================================================
//
// The ultimate code pointer: Fully customizable missiles!
//
//==========================================================================
void A_CustomMissile(AActor * self)
{
	int index=CheckIndex(6);
	if (index<0) return;

	const char * MissileName=(const char*)StateParameters[index];
	fixed_t SpawnHeight=EvalExpressionF (StateParameters[index+1], self) * FRACUNIT;
	int Spawnofs_XY=EvalExpressionI (StateParameters[index+2], self);
	angle_t Angle=EvalExpressionF (StateParameters[index+3], self) * ANGLE_1;
	int aimmode=EvalExpressionI (StateParameters[index+4], self);
	angle_t pitch=EvalExpressionF (StateParameters[index+5], self) * ANGLE_1;

	AActor * targ;
	AActor * missile;

	if (self->target != NULL || aimmode==2)
	{
		const TypeInfo * ti=TypeInfo::FindType(MissileName);
		if (ti) 
		{
			angle_t ang = (self->angle - ANGLE_90) >> ANGLETOFINESHIFT;
			fixed_t x = Spawnofs_XY * finecosine[ang];
			fixed_t y = Spawnofs_XY * finesine[ang];
			fixed_t z = SpawnHeight-32*FRACUNIT;

			switch (aimmode)
			{
			case 0:
				// same adjustment as above (in all 3 directions this time) - for better aiming!
				self->x+=x;
				self->y+=y;
				self->z+=z;
				missile = P_SpawnMissile(self, self->target, ti);
				self->x-=x;
				self->y-=y;
				self->z-=z;
				break;

			case 1:
				missile = P_SpawnMissileXYZ(self->x+x, self->y+y, self->z+SpawnHeight, self, self->target, ti);
				break;

			case 2:
				missile = P_SpawnMissileAngleZ(self, self->z+SpawnHeight, ti, self->angle, 0);

				// It is not necessary to use the correct angle here.
				// The only important thing is that the horizontal momentum is correct.
				// Therefore use 0 as the missile's angle and simplify the calculations accordingly.
				// The actual momentum vector is set below.
				if (missile)
				{
					fixed_t vx = finecosine[pitch>>ANGLETOFINESHIFT];
					fixed_t vz = finesine[pitch>>ANGLETOFINESHIFT];

					missile->momx = FixedMul (vx, missile->Speed);
					missile->momy = 0;
					missile->momz = FixedMul (vz, missile->Speed);
				}

				break;
			}

			if (missile)
			{
				// Use the actual momentum instead of the missile's Speed property
				// so that this can handle missiles with a high vertical velocity 
				// component properly.
				vec3_t velocity = { missile->momx, missile->momy, 0 };

				fixed_t missilespeed=(fixed_t)VectorLength(velocity);

				missile->angle += Angle;
				ang = missile->angle >> ANGLETOFINESHIFT;
				missile->momx = FixedMul (missilespeed, finecosine[ang]);
				missile->momy = FixedMul (missilespeed, finesine[ang]);
	
				// handle projectile shooting projectiles - track the
				// links back to a real owner
                if (self->flags&MF_MISSILE)
                {
                	AActor * owner=self ;//->target;
                	while (owner->flags&MF_MISSILE && owner->target) owner=owner->target;
                	targ=owner;
                	missile->target=owner;
					// automatic handling of seeker missiles
					if (self->flags & missile->flags2 & MF2_SEEKERMISSILE)
					{
						missile->tracer=self->tracer;
					}
                }
				else if (missile->flags2&MF2_SEEKERMISSILE)
				{
					// automatic handling of seeker missiles
					missile->tracer=self->target;
				}
				// set the health value so that the missile works properly
				if (missile->flags4&MF4_SPECTRAL)
				{
					missile->health=-2;
				}
			}
		}
	}
}

//==========================================================================
//
// An even more customizable hitscan attack
//
//==========================================================================
void A_CustomBulletAttack (AActor *self)
{
	int index=CheckIndex(6);
	if (index<0) return;

	angle_t Spread_XY=EvalExpressionF (StateParameters[index], self) * ANGLE_1;
	angle_t Spread_Z=EvalExpressionF (StateParameters[index+1], self) * ANGLE_1;
	int NumBullets=EvalExpressionI (StateParameters[index+2], self);
	int DamagePerBullet=EvalExpressionI (StateParameters[index+3], self);
	const char * PuffType=(const char *)StateParameters[index+4];
	fixed_t Range = EvalExpressionF (StateParameters[index+5], self) * FRACUNIT;

	if(Range==0) Range=MISSILERANGE;


	int i;
	int bangle;
	int bslope;
	const TypeInfo *pufftype;

	if (self->target)
	{
		A_FaceTarget (self);
		bangle = self->angle;

		pufftype = TypeInfo::FindType(PuffType);
		if (!pufftype) pufftype=RUNTIME_CLASS(ABulletPuff);

		bslope = P_AimLineAttack (self, bangle, MISSILERANGE);

		S_SoundID (self, CHAN_WEAPON, self->AttackSound, 1, ATTN_NORM);
		for (i=0 ; i<NumBullets ; i++)
		{
			int angle = bangle + pr_cabullet.Random2() * (Spread_XY / 255);
			int slope = bslope + pr_cabullet.Random2() * (Spread_Z / 255);
			int damage = ((pr_cabullet()%3)+1) * DamagePerBullet;
			P_LineAttack(self, angle, Range, slope, damage, GetDefaultByType(pufftype)->DamageType, pufftype);
		}
    }
}

//==========================================================================
//
// State jump function
//
//==========================================================================
void A_JumpIfNoAmmo(AActor * self)
{
	FState * CallingState;
	int index=CheckIndex(1, &CallingState);
	if (index<0 || !self->player || !self->player->ReadyWeapon) return;	// only for weapons!

	if (!self->player->ReadyWeapon->CheckAmmo(self->player->ReadyWeapon->bAltFire, false, true))
		DoJump(self, CallingState, StateParameters[index]);
}


//==========================================================================
//
// An even more customizable hitscan attack
//
//==========================================================================
void A_FireBullets (AActor *self)
{
	int index=CheckIndex(7);
	if (index<0 || !self->player) return;

	angle_t Spread_XY=EvalExpressionF (StateParameters[index], self) * ANGLE_1;
	angle_t Spread_Z=EvalExpressionF (StateParameters[index+1], self) * ANGLE_1;
	int NumberOfBullets=EvalExpressionI (StateParameters[index+2], self);
	int DamagePerBullet=EvalExpressionI (StateParameters[index+3], self);
	const char * PuffTypeName=(const char *)StateParameters[index+4];
	bool UseNoAmmo=!EvalExpressionI (StateParameters[index+5], self);
	fixed_t Range=EvalExpressionF (StateParameters[index+6], self) * FRACUNIT; 
	
	const TypeInfo * PuffType;

	player_t * player=self->player;
	AWeapon * weapon=player->ReadyWeapon;

	int i;
	int bangle;
	int bslope;

	if (!UseNoAmmo && weapon)
	{
		if (!weapon->DepleteAmmo(weapon->bAltFire, true)) return;	// out of ammo
	}
	
	if (Range == 0) Range = PLAYERMISSILERANGE;

	static_cast<APlayerPawn *>(self)->PlayAttacking2 ();

	P_BulletSlope(self);
	bangle = self->angle;
	bslope = bulletpitch;

	PuffType = TypeInfo::FindType(PuffTypeName);
	if (!PuffType) PuffType=RUNTIME_CLASS(ABulletPuff);

	S_SoundID (self, CHAN_WEAPON, weapon->AttackSound, 1, ATTN_NORM);

	if ((NumberOfBullets==1 && !player->refire) || NumberOfBullets==0)
	{
		int damage = ((pr_cwbullet()%3)+1)*DamagePerBullet;
		P_LineAttack(self, bangle, Range, bslope, damage, GetDefaultByType(PuffType)->DamageType, PuffType);
	}
	else 
	{
		if (NumberOfBullets == -1) NumberOfBullets = 1;
		for (i=0 ; i<NumberOfBullets ; i++)
		{
			int angle = bangle + pr_cwbullet.Random2() * (Spread_XY / 255);
			int slope = bslope + pr_cwbullet.Random2() * (Spread_Z / 255);
			int damage = ((pr_cwbullet()%3)+1) * DamagePerBullet;
			P_LineAttack(self, angle, Range, slope, damage, GetDefaultByType(PuffType)->DamageType, PuffType);
		}
	}
}


//==========================================================================
//
// A_FireProjectile
//
//==========================================================================
void A_FireCustomMissile (AActor * self)
{
	int index=CheckIndex(5);
	if (index<0 || !self->player) return;

	const char * MissileName=(const char *)StateParameters[index];
	angle_t Angle=EvalExpressionF (StateParameters[index+1], self) * ANGLE_1;
	bool UseNoAmmo=!EvalExpressionI (StateParameters[index+2], self);
	int SpawnOfs_XY=EvalExpressionI (StateParameters[index+3], self);
	fixed_t SpawnHeight=EvalExpressionF (StateParameters[index+4], self) * FRACUNIT;

	player_t *player=self->player;
	AWeapon * weapon=player->ReadyWeapon;

	if (!UseNoAmmo && weapon)
	{
		if (!weapon->DepleteAmmo(weapon->bAltFire, true)) return;	// out of ammo
	}

	const TypeInfo * ti=TypeInfo::FindType(MissileName);
	if (ti) 
	{
		angle_t ang = (self->angle - ANGLE_90) >> ANGLETOFINESHIFT;
		fixed_t x = SpawnOfs_XY * finecosine[ang];
		fixed_t y = SpawnOfs_XY * finesine[ang];
		fixed_t z = SpawnHeight;

		AActor * misl=P_SpawnPlayerMissile (self, self->x+x, self->y+y, self->z+z, ti, self->angle);
		// automatic handling of seeker missiles
		if (misl)
		{
			vec3_t velocity = { misl->momx, misl->momy, 0 };
			fixed_t missilespeed=(fixed_t)VectorLength(velocity);

			if (linetarget && misl->flags2&MF2_SEEKERMISSILE) misl->tracer=linetarget;
			misl->angle += Angle;
			angle_t an = misl->angle >> ANGLETOFINESHIFT;
			misl->momx = FixedMul (missilespeed, finecosine[an]);
			misl->momy = FixedMul (missilespeed, finesine[an]);
			if (misl->flags4&MF4_SPECTRAL) misl->health=-1;
		}
	}
}


//==========================================================================
//
// A_CustomPunch
//
// Berserk is not handled here. That can be done with A_CheckIfInventory
//
//==========================================================================
void A_CustomPunch (AActor *self)
{
	int index=CheckIndex(5);
	if (index<0 || !self->player) return;

	int Damage=EvalExpressionI (StateParameters[index], self);
	bool norandom=!!EvalExpressionI (StateParameters[index+1], self);
	bool UseNoAmmo=!EvalExpressionI (StateParameters[index+2], self);
	const char * PuffTypeName=(const char *)StateParameters[index+3];
	fixed_t Range=EvalExpressionF (StateParameters[index+4], self) * FRACUNIT; 

	const TypeInfo * PuffType;


	player_t *player=self->player;
	AWeapon * weapon=player->ReadyWeapon;


	angle_t 	angle;
	int 		pitch;

	if (!norandom) Damage *= (pr_cwpunch()%8+1);

	angle = self->angle + (pr_cwpunch.Random2() << 18);
	pitch = P_AimLineAttack (self, angle, MELEERANGE);

	// only use ammo when actually hitting something!
	if (!UseNoAmmo && linetarget && weapon)
	{
		if (!weapon->DepleteAmmo(weapon->bAltFire, true)) return;	// out of ammo
	}

	PuffType = TypeInfo::FindType(PuffTypeName);
	if (!PuffType) PuffType=RUNTIME_CLASS(ABulletPuff);

	if (Range == 0) Range = MELEERANGE;
	P_LineAttack (self, angle, Range, pitch, Damage, GetDefaultByType(PuffType)->DamageType, PuffType);

	// turn to face target
	if (linetarget)
	{
		S_SoundID (self, CHAN_WEAPON, weapon->AttackSound, 1, ATTN_NORM);

		self->angle = R_PointToAngle2 (self->x,
										self->y,
										linetarget->x,
										linetarget->y);
	}
}


//==========================================================================
//
// customizable railgun attack function
//
//==========================================================================
void A_RailAttack (AActor * self)
{
	int index=CheckIndex(7);
	if (index<0 || !self->player) return;

	int Damage=EvalExpressionI (StateParameters[index], self);
	int Spawnofs_XY=EvalExpressionI (StateParameters[index+1], self);
	bool UseNoAmmo=!EvalExpressionI (StateParameters[index+2], self);
	int Color1=StateParameters[index+3];
	int Color2=StateParameters[index+4];
	bool Silent=!EvalExpressionI (StateParameters[index+5], self);
	float MaxDiff=EvalExpressionF (StateParameters[index+6], self);

	AWeapon * weapon=self->player->ReadyWeapon;

	// only use ammo when actually hitting something!
	if (!UseNoAmmo)
	{
		if (!weapon->DepleteAmmo(weapon->bAltFire, true)) return;	// out of ammo
	}

	// Need to keep the default colors
	if (Color1==0) Color1 = ColorMatcher.Pick (0, 0, 255);
	else if (Color1>0) Color1--;

	if (Color2==0) Color2 = ColorMatcher.Pick (255, 255, 255);
	else if (Color2>0) Color2--;

	P_RailAttack (self, Damage, Spawnofs_XY/*, Color1, Color2, MaxDiff, Silent*/);
}

//==========================================================================
//
// also for monsters
//
//==========================================================================

void A_CustomRailgun (AActor *actor)
{
	if (!actor->target)
		return;

	int index=CheckIndex(7);
	if (index<0) return;

	int Damage=EvalExpressionI (StateParameters[index], actor);
	int Spawnofs_XY=EvalExpressionI (StateParameters[index+1], actor);
	int Color1=StateParameters[index+2];
	int Color2=StateParameters[index+3];
	bool Silent=!EvalExpressionI (StateParameters[index+4], actor);
	bool aim=!EvalExpressionI (StateParameters[index+5], actor);
	float MaxDiff=EvalExpressionF (StateParameters[index+6], actor);

	// [RH] Andy Baker's stealth monsters
	if (actor->flags & MF_STEALTH)
	{
		actor->visdir = 1;
	}

	actor->flags &= ~MF_AMBUSH;
		
	if (aim)
	{
		actor->angle = R_PointToAngle2 (actor->x,
										actor->y,
										actor->target->x,
										actor->target->y);
	}

	actor->pitch = P_AimLineAttack (actor, actor->angle, MISSILERANGE);

	// Let the aim trail behind the player
	if (aim)
	{
		actor->angle = R_PointToAngle2 (actor->x,
										actor->y,
										actor->target->x - actor->target->momx * 3,
										actor->target->y - actor->target->momy * 3);

		if (actor->target->flags & MF_SHADOW)
		{
			actor->angle += pr_crailgun.Random2() << 21;
		}
	}

	// Need to keep the default colors
	if (Color1==0) Color1 = ColorMatcher.Pick (0, 0, 255);
	else if (Color1>0) Color1--;

	if (Color2==0) Color2 = ColorMatcher.Pick (255, 255, 255);
	else if (Color2>0) Color2--;

	P_RailAttack (actor, Damage, Spawnofs_XY/*, Color1, Color2, MaxDiff, Silent*/);
}

//===========================================================================
//
// A_GiveInventory
//
//===========================================================================

void A_GiveInventory(AActor * self)
{
	int index=CheckIndex(2);
	if (index<0) return;

	const char * item =(const char*)StateParameters[index];
	int amount=EvalExpressionI (StateParameters[index+1], self);

	if (amount==0) amount=1;
	const TypeInfo * mi=TypeInfo::FindType(item);
	if (mi) 
	{
		AInventory *item = static_cast<AInventory *>(Spawn (mi, 0, 0, 0));
		item->Amount = amount;
		item->flags |= MF_DROPPED;
		if (item->flags & MF_COUNTITEM)
		{
			item->flags&=~MF_COUNTITEM;
			level.total_items--;
		}
		if (!item->TryPickup (self))
		{
			item->Destroy ();
			StateCall.Result = false;
		}
		else StateCall.Result = true;
	}
	else StateCall.Result = false;
}	

//===========================================================================
//
// A_GiveInventory
//
//===========================================================================

void A_TakeInventory(AActor * self)
{
	int index=CheckIndex(2);
	if (index<0) return;

	const char * item =(const char*)StateParameters[index];
	int amount=EvalExpressionI (StateParameters[index+1], self);

	const TypeInfo * mi=TypeInfo::FindType(item);

	StateCall.Result=false;
	if (mi) 
	{
		AInventory * inv = self->FindInventory(mi);

		if (inv && !inv->IsKindOf(RUNTIME_CLASS(AHexenArmor)))
		{
			if (inv->Amount > 0) StateCall.Result=true;
			if (!amount || amount>=inv->Amount) 
			{
				if (inv->IsKindOf(RUNTIME_CLASS(AAmmo))) inv->Amount=0;
				else inv->Destroy();
			}
			else inv->Amount-=amount;
		}
	}
}	


//===========================================================================
//
// A_SpawnItem
//
// Spawns an item in front of the caller like Heretic's time bomb
//
//===========================================================================
void A_SpawnItem(AActor * self)
{
	FState * CallingState;
	int index=CheckIndex(4, &CallingState);
	if (index<0) return;

	const TypeInfo * missile= TypeInfo::FindType((const char *)StateParameters[index]);
	int distance = EvalExpressionI (StateParameters[index+1], self);
	fixed_t zheight = EvalExpressionF (StateParameters[index+2], self) * FRACUNIT;
	bool useammo = !EvalExpressionI (StateParameters[index+3], self);

	if (!missile) 
	{
		StateCall.Result=false;
		return;
	}

	if (distance==0) 
	{
		// use the minimum distance that does not result in an overlap
		distance=(self->radius+GetDefaultByType(missile)->radius)>>FRACBITS;
	}

	if (self->player && CallingState != self->state && CallingState != StateCall.State)
	{
		// Used from a weapon so use some ammo
		AWeapon * weapon=self->player->ReadyWeapon;

		if (!weapon) return;
		if (useammo && !weapon->DepleteAmmo(weapon->bAltFire)) return;
	}

	AActor * mo = Spawn( missile, 
					self->x + distance*finecosine[self->angle>>ANGLETOFINESHIFT], 
					self->y + distance*finesine[self->angle>>ANGLETOFINESHIFT], 
					self->z - self->floorclip + zheight);

	mo->angle=self->angle;
	if (mo) 
	{
		AActor * originator = self;

		while (originator && originator->flags&MF_MISSILE) originator = originator->target;

		if (mo->flags3&MF3_ISMONSTER)
		{
			if (!P_TestMobjLocation(mo))
			{
				// The monster is blocked so don't spawn it at all!
				if (mo->flags&MF_COUNTKILL) level.total_monsters--;
				mo->Destroy();
				StateCall.Result=false;	// for an inventory iten's use state
				return;
			}
			else if (originator)
			{
				if (originator->flags3&MF3_ISMONSTER)
				{
					// If this is a monster transfer all friendliness information
					mo->target = originator->target;
					mo->TIDtoHate = originator->TIDtoHate;
					mo->LastLook = originator->LastLook;
					mo->flags3 |= originator->flags3 & (MF3_NOSIGHTCHECK | MF3_HUNTPLAYERS);
					mo->flags4 |= originator->flags4 & MF4_NOHATEPLAYERS;
					mo->flags = (mo->flags & ~MF_FRIENDLY) | (originator->flags & MF_FRIENDLY);
					// Note to Randy: This line is missing from all other friendliness transfers!
					mo->FriendPlayer=originator->FriendPlayer;

					if (useammo) mo->master = originator;	// don't let it attack you (optional)!
				}
				else if (originator->player)
				{
					// A player always spawns a monster friendly to him
					mo->flags|=MF_FRIENDLY;
					mo->FriendPlayer = originator->player-players+1;

					AActor * attacker=originator->player->attacker;
					if (attacker)
					{
						if (!(attacker->flags&MF_FRIENDLY) || 
							(deathmatch && attacker->FriendPlayer!=0 && attacker->FriendPlayer!=mo->FriendPlayer))
						{
							// Target the monster which last attacked the player
							mo->target = attacker;
						}
					}
				}
			}
		}
		else 
		{
			// If this is a missile or something else set the target to the originator
			mo->target=originator? originator : self;
		}
	}
	StateCall.Result=true;
}

//===========================================================================
//
// A_ThrowGrenade
//
// Throws a grenade (like Hexen's fighter flechette)
//
//===========================================================================
void A_ThrowGrenade(AActor * self)
{
	FState * CallingState;
	int index=CheckIndex(5, &CallingState);
	if (index<0) return;

	const TypeInfo * missile= TypeInfo::FindType((const char *)StateParameters[index]);
	fixed_t zheight = EvalExpressionF (StateParameters[index+1], self) * FRACUNIT;
	fixed_t xymom = EvalExpressionF (StateParameters[index+2], self) * FRACUNIT;
	fixed_t zmom = EvalExpressionF (StateParameters[index+3], self) * FRACUNIT;
	bool useammo = !EvalExpressionI (StateParameters[index+4], self);

	if (self->player && CallingState != self->state && CallingState != StateCall.State)
	{
		// Used from a weapon so use some ammo
		AWeapon * weapon=self->player->ReadyWeapon;

		if (!weapon) return;
		if (useammo && !weapon->DepleteAmmo(weapon->bAltFire)) return;
	}


	AActor * bo;

	bo = Spawn(missile, self->x, self->y, self->z - self->floorclip + zheight + 35*FRACUNIT);
	if (bo)
	{
		int pitch = self->pitch;

		if (xymom) bo->Speed=xymom;
		bo->angle = self->angle+(((pr_grenade()&7)-4)<<24);
		bo->momz = zmom + 2*finesine[pitch>>ANGLETOFINESHIFT];
		bo->z += 2 * finesine[pitch>>ANGLETOFINESHIFT];
		P_ThrustMobj(bo, bo->angle, bo->Speed);
		bo->momx += self->momx>>1;
		bo->momy += self->momy>>1;
		bo->target= self;
		bo->tics -= pr_grenade()&3;
		P_CheckMissileSpawn (bo);
		StateCall.Result=true;
	} 
	else StateCall.Result=false;
}


//===========================================================================
//
// A_Recoil
//
//===========================================================================
void A_Recoil(AActor * actor)
{
	int index=CheckIndex(1, NULL);
	if (index<0) return;
	fixed_t xymom = EvalExpressionF (StateParameters[index], actor) * FRACUNIT;

	angle_t angle = actor->angle + ANG180;
	angle >>= ANGLETOFINESHIFT;
	actor->momx += FixedMul (xymom, finecosine[angle]);
	actor->momy += FixedMul (xymom, finesine[angle]);
}


//===========================================================================
//
// A_SelectWeapon
//
//===========================================================================
void A_SelectWeapon(AActor * actor)
{
	int index=CheckIndex(1, NULL);
	if (index<0 || actor->player == NULL) return;

	const TypeInfo * weapon= TypeInfo::FindType((const char *)StateParameters[index]);
	AWeapon * weaponitem = static_cast<AWeapon*>(actor->FindInventory(weapon));

	if (weaponitem != NULL && weaponitem->IsKindOf(RUNTIME_CLASS(AWeapon)))
	{
		if (actor->player->ReadyWeapon != weaponitem)
		{
			actor->player->PendingWeapon = weaponitem;
		}
		StateCall.Result=true;
	}
	else StateCall.Result=false;
}

//===========================================================================
//
// A_Print
//
//===========================================================================
void A_Print(AActor * actor)
{
	int index=CheckIndex(1, NULL);
	if (index<0) return;

	if (actor->CheckLocalView (consoleplayer) ||
		(actor->target!=NULL && actor->target->CheckLocalView (consoleplayer)))
	{
		C_MidPrint((const char *)StateParameters[index]);
	}
}


//===========================================================================
//
// A_SetTranslucent
//
//===========================================================================
void A_SetTranslucent(AActor * self)
{
	int index=CheckIndex(2, NULL);
	if (index<0) return;

	fixed_t alpha = EvalExpressionF (StateParameters[index], self) * FRACUNIT;
	int mode = EvalExpressionI (StateParameters[index+1], self);
	mode = mode == 0 ? STYLE_Translucent : mode == 2 ? STYLE_Fuzzy : STYLE_Add;

	self->alpha=clamp<fixed_t>(alpha, 0, FRACUNIT);

	if (mode != STYLE_Fuzzy)
	{
		if (self->alpha == 0) mode = STYLE_None;
		else if (mode == STYLE_Translucent && self->alpha == FRACUNIT) mode = STYLE_Normal;
	}

	self->RenderStyle=mode;
}

//===========================================================================
//
// A_FadeOut
//
// fades the actor out and destroys it when done
//
//===========================================================================
void A_FadeOut(AActor * self)
{
	int index=CheckIndex(1, NULL);
	if (index<0) return;

	fixed_t reduce = EvalExpressionF (StateParameters[index], self) * FRACUNIT;
	if (reduce == 0) reduce = FRACUNIT/10;

	if (self->RenderStyle==STYLE_Normal) self->RenderStyle=STYLE_Translucent;
	self->alpha -= reduce;
	if (self->alpha<=0) self->Destroy();
}

//===========================================================================
//
// A_SpawnDebris
//
//===========================================================================
void A_SpawnDebris(AActor * self)
{
	int i;
	AActor * mo;
	const TypeInfo * debris;

	int index=CheckIndex(1, NULL);
	if (index<0) return;

	debris = TypeInfo::FindType((const char *)StateParameters[index]);
	if (debris == NULL) return;

	for (i = 0; i < GetDefaultByType(debris)->health; i++)
	{
		mo = Spawn(debris, self->x+((pr_spawndebris()-128)<<12),
			self->y+((pr_spawndebris()-128)<<12), 
			self->z+(pr_spawndebris()*self->height/256));
		if (mo && i < mo->GetClass()->ActorInfo->NumOwnedStates)
		{
			mo->SetState (mo->GetClass()->ActorInfo->OwnedStates + i);
			mo->momz = ((pr_spawndebris()&7)+5)*FRACUNIT;
			mo->momx = pr_spawndebris.Random2()<<(FRACBITS-6);
			mo->momy = pr_spawndebris.Random2()<<(FRACBITS-6);
		}
	}
}


//===========================================================================
//
// A_CheckSight
// jumps if no player can see this actor
//
//===========================================================================
void A_CheckSight(AActor * self)
{
	for (int i=0;i<MAXPLAYERS;i++) 
	{
		if (playeringame[i] && P_CheckSight(players[i].camera,self,true)) return;
	}

	FState * CallingState;
	int index=CheckIndex(1, &CallingState);

	if (index>=0) DoJump(self, CallingState, StateParameters[index]);
}


//===========================================================================
//
// A_ExtChase
// A_Chase with optional parameters
//
//===========================================================================
void A_DoChase(AActor * actor, bool fastchase, FState * meleestate, FState * missilestate, bool playactive, bool nightmarefast);

void A_ExtChase(AActor * self)
{
	int index=CheckIndex(4, &CallingState);

	A_DoChase(self, false,
		EvalExpressionI (StateParameters[index], self) ? self->MeleeState:NULL,
		EvalExpressionI (StateParameters[index+1], self) ? self->MissileState:NULL,
		!EvalExpressionI (StateParameters[index+2], self),
		!!EvalExpressionI (StateParameters[index+3], self));
}


//===========================================================================
//
// Weapon jiggling
//
//===========================================================================
void A_Jiggle(AActor * self)
{
	int index=CheckIndex(2, &CallingState);
	int xmax = EvalExpressionI (StateParameters[index], self);
	int ymax = EvalExpressionI (StateParameters[index+1], self);

	if (self->player)
	{
		int rand_x = (pr_jiggle()%(xmax*2))-xmax;
		int rand_y = (pr_jiggle()%(ymax*2))-ymax;
		self->player->psprites[0].sx += rand_x;
		self->player->psprites[0].sy += rand_y;
		self->player->psprites[1].sx += rand_x;
		self->player->psprites[1].sy += rand_y;
	}
}

//===========================================================================
//
// Inventory drop
//
//===========================================================================
void A_DropInventory(AActor * self)
{
	int index=CheckIndex(1, &CallingState);
	const TypeInfo * ti = TypeInfo::FindType((const char*)StateParameters[index]);
	if (ti)
	{
		AInventory * inv = self->FindInventory(ti);
		if (inv)
		{
			self->DropInventory(inv);
		}
	}
}


//===========================================================================
//
// A_SetBlend
//
//===========================================================================
void A_SetBlend(AActor * self)
{
	int index=CheckIndex(3);
	int colorn = StateParameters[index];

	PalEntry color;
	if (!colorn)
		return;
	else if (colorn == -1)
		color.r = color.g = color.b = 255;
	else
		color = GPalette.BaseColors[colorn];

	fixed_t alpha = EvalExpressionF (StateParameters[index+1], self) * FRACUNIT;
	int tics = EvalExpressionI (StateParameters[index+2], self);


	new DFlashFader(color.r/255.0f, color.g/255.0f, color.b/255.0f, alpha/(float)FRACUNIT,
					color.r/255.0f, color.g/255.0f, color.b/255.0f, 0, tics, self);
}


//===========================================================================
//
// A_JUmpIf
//
//===========================================================================
void A_JumpIf(AActor * self)
{
	FState * CallingState;
	int index=CheckIndex(2, &CallingState);
	int expression = EvalExpressionI (StateParameters[index], self);

	if (index>=0 && expression) DoJump(self, CallingState, StateParameters[index+1]);
}

//===========================================================================
//
// A_KillMaster
//
//===========================================================================
void A_KillMaster(AActor * self)
{
	if (self->master)
	{
		P_DamageMobj(self->master, self, self, self->master->health, MOD_UNKNOWN, DMG_NO_ARMOR);
	}
}

//===========================================================================
//
// A_KillChildren
//
//===========================================================================
void A_KillChildren(AActor * self)
{
	TThinkerIterator<AActor> it;
	AActor * mo;

	while (mo=it.Next())
	{
		if (mo->master == self)
		{
			P_DamageMobj(mo, self, self, mo->health, MOD_UNKNOWN, DMG_NO_ARMOR);
		}
	}
}


