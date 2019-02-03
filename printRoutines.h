/* This file contains the prototypes and constants needed to use the
   routines defined in printRoutines.c
*/

#ifndef _PRINTROUTINES_H_
#define _PRINTROUTINES_H_

#include <stdio.h>

// int printPosition(FILE *, unsigned long);
int printPositionFirstLine(FILE *, unsigned long);
int printInstruction(FILE *);

////mine functions
int printPositionNotFirstLine(FILE *, unsigned long);
int pHalt(FILE *);
int pNop(FILE *);
int pRrmovq(FILE *, int, int); 
int pCmov(FILE *, int, int, int);
int pIrmovq(FILE *, int, unsigned long, unsigned long);
int pRmmovq(FILE *, int, int, unsigned long, unsigned long);
int pMrmovq(FILE *, int, int, unsigned long, unsigned long);
int pOpq(FILE *, int, int, int);
int pJump(FILE *, int, unsigned long, unsigned long);
int pCall(FILE *, unsigned long, unsigned long);
int pRet(FILE *);
int pPushq(FILE *, int);
int pPopq(FILE *, int);    


////end of mine

#endif /* PRINTROUTINES */
