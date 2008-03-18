
#ifndef __3DMIDTEX_H
#define __3DMIDTEX_H

#include "doomtype.h"
#include "r_defs.h"

class AActor;

bool P_Scroll3dMidtex(sector_t *sector, int crush, fixed_t move, bool ceiling);
void P_Start3dMidtexInterpolations(sector_t *sec, bool ceiling);
void P_Stop3dMidtexInterpolations(sector_t *sec, bool ceiling);
void P_Attach3dMidtexLinesToSector(sector_t *dest, int lineid, int tag, bool ceiling);
bool P_GetMidTexturePosition(const line_t *line, int sideno, fixed_t *ptextop, fixed_t *ptexbot);
bool P_Check3dMidSwitch(AActor *actor, line_t *line, int side);
bool P_LineOpening_3dMidtex(AActor *thing, const line_t *linedef, fixed_t &opentop, fixed_t &openbottom);


#endif