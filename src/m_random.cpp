// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id: m_random.c,v 1.6 1998/05/03 23:13:18 killough Exp $
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
//
// DESCRIPTION:
//      Random number LUT.
//
// 1/19/98 killough: Rewrote random number generator for better randomness,
// while at the same time maintaining demo sync and backward compatibility.
//
// 2/16/98 killough: Made each RNG local to each control-equivalent block,
// to reduce the chances of demo sync problems.
//
// [RH] Changed to use different class instances for different RNGs. Be
// sure to compile with _DEBUG if you want to catch bad RNG names.
//
//-----------------------------------------------------------------------------

#include <assert.h>

#include "doomstat.h"
#include "doomdef.h"
#include "m_random.h"
#include "farchive.h"
#include "b_bot.h"
#include "m_png.h"
#include "m_crc32.h"
#include "i_system.h"
#include "c_dispatch.h"
#include "files.h"

#define RAND_ID MAKE_ID('r','a','N','d')

// Doom's original random number generator.

//
// M_Random
// Returns a 0-255 number
//
unsigned char rndtable[256] = {
	0,	 8, 109, 220, 222, 241, 149, 107,  75, 248, 254, 140,  16,	66 ,
	74,  21, 211,  47,	80, 242, 154,  27, 205, 128, 161,  89,	77,  36 ,
	95, 110,  85,  48, 212, 140, 211, 249,	22,  79, 200,  50,	28, 188 ,
	52, 140, 202, 120,	68, 145,  62,  70, 184, 190,  91, 197, 152, 224 ,
	149, 104,  25, 178, 252, 182, 202, 182, 141, 197,	4,	81, 181, 242 ,
	145,  42,  39, 227, 156, 198, 225, 193, 219,  93, 122, 175, 249,   0 ,
	175, 143,  70, 239,  46, 246, 163,	53, 163, 109, 168, 135,   2, 235 ,
	25,  92,  20, 145, 138,  77,  69, 166,	78, 176, 173, 212, 166, 113 ,
	94, 161,  41,  50, 239,  49, 111, 164,	70,  60,   2,  37, 171,  75 ,
	136, 156,  11,	56,  42, 146, 138, 229,  73, 146,  77,	61,  98, 196 ,
	135, 106,  63, 197, 195,  86,  96, 203, 113, 101, 170, 247, 181, 113 ,
	80, 250, 108,	7, 255, 237, 129, 226,	79, 107, 112, 166, 103, 241 ,
	24, 223, 239, 120, 198,  58,  60,  82, 128,   3, 184,  66, 143, 224 ,
	145, 224,  81, 206, 163,  45,  63,	90, 168, 114,  59,	33, 159,  95 ,
	28, 139, 123,  98, 125, 196,  15,  70, 194, 253,  54,  14, 109, 226 ,
	71,  17, 161,  93, 186,  87, 244, 138,	20,  52, 123, 251,	26,  36 ,
	17,  46,  52, 231, 232,  76,  31, 221,	84,  37, 216, 165, 212, 106 ,
	197, 242,  98,	43,  39, 175, 254, 145, 190,  84, 118, 222, 187, 136 ,
	120, 163, 236, 249
};

int 	prndindex = 0;

FRandom M_Random;

inline int UpdateSeed (DWORD &seed)
{
	DWORD oldseed = seed;
	seed = oldseed * 1664525ul + 221297ul;
	return (int)oldseed;
}

DWORD rngseed = 1993;   // killough 3/26/98: The seed

FRandom *FRandom::RNGList;

unsigned int P_Random (void)
{
	prndindex = (prndindex+1)&0xff;
	return rndtable[prndindex];
}

void M_ClearRandom (void)
{
	prndindex = 0;
}

FRandom::FRandom ()
: Seed (0), Next (NULL), NameCRC (0), useOldRNG (false)
{
#ifdef _DEBUG
	Name = NULL;
#endif
	Next = RNGList;
	RNGList = this;
}

FRandom::FRandom (const char *name, bool useold)
: Seed (0)
{
	NameCRC = CalcCRC32 ((const BYTE *)name, (unsigned int)strlen (name));
	useOldRNG = useold;
#ifdef _DEBUG
	Name = name;
	// A CRC of 0 is reserved for nameless RNGs that don't get stored
	// in savegames. The chance is very low that you would get a CRC of 0,
	// but it's still possible.
	assert (NameCRC != 0);
#endif

	// Insert the RNG in the list, sorted by CRC
	FRandom **prev = &RNGList, *probe = RNGList;

	while (probe != NULL && probe->NameCRC < NameCRC)
	{
		prev = &probe->Next;
		probe = probe->Next;
	}

#ifdef _DEBUG
	if (probe != NULL)
	{
		// Because RNGs are identified by their CRCs in save games,
		// no two RNGs can have names that hash to the same CRC.
		// Obviously, this means every RNG must have a unique name.
		assert (probe->NameCRC != NameCRC);
	}
#endif

	Next = probe;
	*prev = this;
}

FRandom::~FRandom ()
{
	FRandom *rng, **prev;

	FRandom *last = NULL;

	prev = &RNGList;
	rng = RNGList;

	while (rng != NULL && rng != this)
	{
		last = rng;
		rng = rng->Next;
	}

	if (rng != NULL)
	{
		*prev = rng->Next;
	}
}

int FRandom::operator() ()
{
	// [BB] Use Doom's original random numbers if the user wants it.
	if (useOldRNG && (compatflags & COMPATF_OLDRANDOMGENERATOR))
		return P_Random();

	return (UpdateSeed (Seed) >> 20) & 255;
}

int FRandom::operator() (int mod)
{
	if (mod <= 256)
	{ // The mod is small enough, so a byte is enough to get a good number.
		return (*this)() % mod;
	}
	else
	{ // For mods > 256, construct a 32-bit int and modulo that.
		int num = (*this)();
		num = (num << 8) | (*this)();
		num = (num << 8) | (*this)();
		num = (num << 8) | (*this)();
		return (num&0x7fffffff) % mod;
	}
}

int FRandom::Random2 ()
{
	// [BB] Use Doom's original random numbers if the user wants it.
	if (useOldRNG && (compatflags & COMPATF_OLDRANDOMGENERATOR))
		return ( P_Random() - P_Random() );
	
	int t = (*this)();
	int u = (*this)();
	return t - u;
}

int FRandom::Random2 (int mask)
{
	int t = (*this)() & mask;
	int u = (*this)() & mask;
	return t - u;
}

int FRandom::HitDice (int count)
{
	return (1 + ((UpdateSeed (Seed) >> 20) & 7)) * count;
}

// Initialize all the seeds
//
// This initialization method is critical to maintaining demo sync.
// Each seed is initialized according to its class. killough
//

void FRandom::StaticClearRandom ()
{
	const DWORD seed = rngseed*2+1;	// add 3/26/98: add rngseed
	FRandom *rng = FRandom::RNGList;

	// go through each RNG and set each starting seed differently
	while (rng != NULL)
	{
		// [RH] Use the RNG's name's CRC to modify the original seed.
		// This way, new RNGs can be added later, and it doesn't matter
		// which order they get initialized in.
		rng->Seed = seed * rng->NameCRC;
		rng = rng->Next;
	}
}

// This function produces a DWORD that can be used to check the consistancy
// of network games between different machines. Only a select few RNGs are
// used for the sum, because not all RNGs are important to network sync.

extern FRandom pr_spawnmobj;
extern FRandom pr_acs;
extern FRandom pr_chase;
extern FRandom pr_lost;
extern FRandom pr_slam;

DWORD FRandom::StaticSumSeeds ()
{
	return pr_spawnmobj.Seed + pr_acs.Seed + pr_chase.Seed + pr_lost.Seed + pr_slam.Seed + prndindex;
}

void FRandom::StaticWriteRNGState (FILE *file)
{
	FRandom *rng;
	const DWORD seed = rngseed*2+1;
	FPNGChunkArchive arc (file, RAND_ID);

	arc << rngseed;

	// Only write those RNGs that have been used
	for (rng = FRandom::RNGList; rng != NULL; rng = rng->Next)
	{
		if (rng->NameCRC != 0 && rng->Seed != seed + rng->NameCRC)
		{
			arc << rng->NameCRC << rng->Seed;
		}
	}
}

void FRandom::StaticReadRNGState (PNGHandle *png)
{
	FRandom *rng;

	size_t len = M_FindPNGChunk (png, RAND_ID);

	if (len != 0)
	{
		const int rngcount = (int)((len-4) / 8);
		int i;
		DWORD crc;

		FPNGChunkArchive arc (png->File->GetFile(), RAND_ID, len);

		arc << rngseed;
		FRandom::StaticClearRandom ();

		for (i = rngcount; i; --i)
		{
			arc << crc;
			for (rng = FRandom::RNGList; rng != NULL; rng = rng->Next)
			{
				if (rng->NameCRC == crc)
				{
					arc << rng->Seed;
					break;
				}
			}
		}
		png->File->ResetFilePtr();
	}
}

#ifdef _DEBUG
void FRandom::StaticPrintSeeds ()
{
	FRandom *rng = RNGList;

	while (rng != NULL)
	{
		Printf ("%s: %08lx\n", rng->Name, rng->Seed);
		rng = rng->Next;
	}
}

CCMD (showrngs)
{
	FRandom::StaticPrintSeeds ();
}
#endif
