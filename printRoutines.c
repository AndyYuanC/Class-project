#include <stdio.h>
#include <unistd.h>

#include <inttypes.h>
#include "printRoutines.h"

/*
  You probably want to create a number of printing routines in this
  file.  Put the prototypes in printRoutines.h.
*/


//registers' names
const char *registers[] = {
	"%rax", "%rcx", "%rdx", "%rbx", "%rsp", "%rbp", "%rsi", "%rdi", 
	"%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14"};

//jump conditions' names
const char *jumpconditions[] = {
	"jmp", "jle", "jl", "je", "jne", "jge", "jg" };

//cmov conditions' names
const char *cmovConditions[] = {
  "yi", "cmovle", "cmovl", "cmove", "cmovne", "cmovge", "cmovg" };

//operations' names
const char *operations[] = {
	"addq", "subq", "andq", "xorq", "mulq", "divq", "modq" };


int printPositionFirstLine(FILE *out, unsigned long location) { // position being printed is the first line in the outpu
  return fprintf(out, ".pos 0x%lx\n", location);
}

/* This is a function to demonstrate how to do print formatting. It
 * takes the file descriptor the I/O is to be done to. You are not
 * required to use the same type of printf formatting, but you must
 * produce the same resulting format. You are also not required to
 * keep this function in your final result, and may replace it with
 * other equivalent instructions.
 *
 * The arguments for the format string in the example printing are
 * strictly for illustrating the spacing. You are free to construct
 * the output however you want.
 */
int printInstruction(FILE *out) {

  int res = 0;
  
  char * r1 = "%rax";
  char * r2 = "%rdx";
  char * inst1 = "rrmovq";
  char * inst2 = "jne";
  char * inst3 = "irmovq";
  char * inst4 = "mrmovq";
  unsigned long destAddr = 8193;
  
  res += fprintf(out, "    %-8s%s, %s          # %-22s\n", 
		 inst1, r1, r2, "2002");

  res += fprintf(out, "    %-8s0x%lx              # %-22s\n", 
		 inst2, destAddr, "740120000000000000");

  res += fprintf(out, "    %-8s$0x%lx, %s         # %-22s\n", 
		 inst3, 16L, r2, "30F21000000000000000");

  res += fprintf(out, "    %-8s0x%lx(%s), %s # %-22s\n", 
		 inst4, 65536L, r2, r1, "50020000010000000000"); 
  
  res += fprintf(out, "    %-8s%s, %s          # %-22s\n", 
		 inst1, r2, r1, "2020");
  
  res += fprintf(out, "    %-8s0x%lx  # %-22s\n", 
		 ".quad", 0xFFFFFFFFFFFFFFFFL, "FFFFFFFFFFFFFFFF");

  return res;
}  
  
int printPositionNotFirstLine(FILE *out, unsigned long location) { // position being printed is not the first line in the outpu
  return fprintf(out, "\n.pos 0x%lx\n", location);
}

int pHalt(FILE *out) {
  int res = 0;
  res += fprintf(out, "    %-8s           # %-22s\n", 
    "halt", "00");
  return res;
}

int pNop(FILE *out) {
  int res = 0;
  res += fprintf(out, "    %-8s           # %-22s\n", 
    "nop", "10");
  return res;
}

int pRrmovq(FILE *out , int rA, int rB) {
  int res = 0;
  res += fprintf(out, "    %-8s%s, %s          # %s%X%X\n", 
     "rrmovq", registers[rA], registers[rB], "20", rA, rB);
  return res;
}


int pCmov(FILE *out , int fn, int rA, int rB) {
  int res = 0;
	res += fprintf(out, "    %-8s%s, %s          # %s%X%X%X\n", 
	cmovConditions[fn], registers[rA], registers[rB], "2", fn, rA, rB);
  return res;
}

int pIrmovq(FILE *out , int rB, unsigned long immediateValue, unsigned long littleEndian) { // rA = F
  int res = 0;
  res += fprintf(out, "    %-8s$0x%lx, %s         # %s%X%016lX\n", 
		 "irmovq", immediateValue, registers[rB], "30F", rB, littleEndian);
  return res;
}

int pRmmovq(FILE *out, int rA, int rB, unsigned long displacement, unsigned long littleEndian) {
  int res = 0;
  res += fprintf(out, "    %-8s%s, 0x%lx(%s)    # %s%X%X%016lX\n", 
     "rmmovq", registers[rA], displacement, registers[rB], "40", rA, rB, littleEndian); 
  return res;
}


int pMrmovq(FILE *out, int rA, int rB, unsigned long displacement, unsigned long littleEndian) {
  int res = 0;
  res += fprintf(out, "    %-8s0x%lx(%s), %s    # %s%X%X%016lX\n", 
     "mrmovq", displacement, registers[rB], registers[rA], "50", rA, rB, littleEndian); 
  return res;
}

int pOpq(FILE *out, int fn, int rA, int rB) {
  int res = 0;
  res += fprintf(out, "    %-8s%s, %s    # %s%X%X%X\n", 
    operations[fn], registers[rA], registers[rB],"6", fn, rA, rB); 
  return res;
}

int pJump(FILE *out, int fn, unsigned long destination, unsigned long littleEndian) {  
  int res = 0;	
  res += fprintf(out, "    %-8s0x%lx              # %s%X%016lX\n", 
		 jumpconditions[fn] , destination, "7", fn, littleEndian);
  return res;
}

int pCall(FILE *out, unsigned long destination, unsigned long littleEndian) {
	int res = 0;
	res += fprintf(out, "    %-8s0x%lx              # %s%016lX\n", 
		   "call", destination, "80", littleEndian);
	return res;
}

int pRet(FILE *out) {
	int res = 0;
  	res += fprintf(out, "    %-8s           # %-22s\n", 
    	   "ret", "90");
  	return res;
}

int pPushq(FILE *out , int rA) {
	int res = 0;
	res += fprintf(out, "    %-8s%s       # %s%X%s\n", 
		   "pushq", registers[rA], "A0", rA, "F"); 
	return res;
}

int pPopq(FILE *out , int rA) {
	int res = 0;
	res += fprintf(out, "    %-8s%s       # %s%X%s\n", 
		   "popq", registers[rA], "B0", rA, "F"); 
	return res;
}


