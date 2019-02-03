#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <stdbool.h>

#include "printRoutines.h"

#define ERROR_RETURN -1
#define SUCCESS 0


unsigned long BigEndianToLittleEndian (unsigned long k) {

   k = (( k << 56) |
       ((k & 0x000000000000FF00) << 40) |
       ((k & 0x0000000000FF0000) << 24) |
       ((k & 0x00000000FF000000) << 8) |
       ((k & 0x000000FF00000000) >> 8) |
       ((k & 0x0000FF0000000000) >> 24)|
       ((k & 0x00FF000000000000) >> 40)|
        (k >> 56));

   return k;
}


int main(int argc, char **argv) {
  
  FILE *machineCode, *outputFile;
  long currAddr = 0; 

  // Verify that the command line has an appropriate number
  // of arguments

  if (argc < 2 || argc > 4) {
    fprintf(stderr, "Usage: %s InputFilename [OutputFilename] [startingOffset]\n", argv[0]);
    return ERROR_RETURN;
  }

  // First argument is the file to read, attempt to open it 
  // for reading and verify that the open did occur.
  machineCode = fopen(argv[1], "rb");

  if (machineCode == NULL) {
    fprintf(stderr, "Failed to open %s: %s\n", argv[1], strerror(errno));
    return ERROR_RETURN;
  }

  // Second argument is the file to write, attempt to open it for
  // writing and verify that the open did occur. Use standard output
  // if not provided.
  outputFile = argc <= 2 ? stdout : fopen(argv[2], "w");
  
  if (outputFile == NULL) {
    fprintf(stderr, "Failed to open %s: %s\n", argv[2], strerror(errno));
    fclose(machineCode);
    return ERROR_RETURN;
  }

  // If there is a 3rd argument present it is an offset so convert it
  // to a numeric value.
  if (4 == argc) {
    errno = 0;
    currAddr = strtol(argv[3], NULL, 0);
    if (errno != 0) {
      perror("Invalid offset on command line");
      fclose(machineCode);
      fclose(outputFile);
      return ERROR_RETURN;
    }
  }

  fprintf(stderr, "Opened %s, starting offset 0x%lX\n", argv[1], currAddr);
  fprintf(stderr, "Saving output to %s\n", argc <= 2 ? "standard output" : argv[2]);

  // Comment or delete the following lines and this comment before  !!!!!!
  // handing in your final version.
  // if (currAddr) printPositionFirstLine(outputFile, currAddr);
  // printInstruction(outputFile);

  // Your code starts here.////////////////////////////////////////////////////////////////
  
  int length; //length of file
  fseek(machineCode, 0, SEEK_END); // set position to the end of file to read the length of file
  length = ftell(machineCode) - 1;
  fseek(machineCode, 0, SEEK_SET); // set position to beginning of file after reading the file size
  

  /////////////////////////////////////////////////////////////////////////////////////////
  // determine the position of the first non-zero byte
  int c = 0;
  fseek(machineCode, currAddr, SEEK_SET); //set offset
  while (c == 0) {
	c = fgetc(machineCode); 
  }


  int posOfFirstNonZeroByte = ftell(machineCode) - 1;
  if ((posOfFirstNonZeroByte == 0) && (currAddr == 0)) {
  } else {
	  currAddr = posOfFirstNonZeroByte;
	  if (posOfFirstNonZeroByte != 0) { // the first non-zero byte found in the file is not the first byte of the file (offset 0)
	  	printPositionFirstLine(outputFile, currAddr);
	  }
  }
  fseek(machineCode, -1, SEEK_CUR);


//////////////////////////////////////////////////////////////////////////////////////////////
while(!feof(machineCode)){ 									   // keep reading 1 byte until reaching end of file

  c = fgetc(machineCode);

  if(c == 0){ 												   // "halt" and consecutive zero case;  

        int endOff = ftell(machineCode) - 1;
        int endOff1 = length - endOff;						   // file contains single zero byte
        if(endOff1 == 1){
          fprintf(outputFile, "    .byte 0x0\n" );
        }

                                          
        pHalt(outputFile);

        //contiguous zero case
        while(c == 0 && endOff1 > 2){							// check if there are two or more zero bytes at the end of file
            c = fgetc(machineCode);
            endOff1 --;
        }

        if((c == 0) && endOff1 == 2 ){
	        fprintf(outputFile, "\n.pos 0x%x\n", length - 1); 
	        fprintf(outputFile, "    .byte 0x0\n" );
	        break;
        } else { 
        	int currentA = ftell(machineCode) - 1;
          	fprintf(outputFile, "\n.pos 0x%x\n", currentA);}

      }

    //first half byte of the 1st byte
  	int iCd = c >> 4;
  	//second half byte of the 1st byte
  	int iFn = c & 0x0f;

  	//bool instructionValidOrNot = InstructionIsValid();
    unsigned long v;
    unsigned long d;
    unsigned long dest;

    //check if instruction is valid
    bool condition1 = true;    // default value is true for conditions 
    bool conditoin2a = true;
    bool condition2b = true;
    bool condition3a = true;
    bool condition3b = true;
    bool condition3c = true;

    condition1 = (0 <= iCd) && (iCd <= 11); //Have a valid opcode in the high order half-byte.

    if ((iCd != 2)&&(iCd != 6)&&(iCd != 7)) {
      conditoin2a = (iFn == 0);          // check whether the second-half byte is 0 when the first half-byte is not 2/6/7 
    }

    if ((iCd == 2)||(iCd == 6)||(iCd == 7)) {
      condition2b = (0 <= iFn)&&(iFn <= 6); 
    }

    bool insValidReadOnlyFirstByte = condition1 && conditoin2a && condition2b;

  if (insValidReadOnlyFirstByte) {
    if (((c >> 4) == 1) || ((c >> 4) == 9)) {
      switch (c>>4) {
        case 0x1:                                               //"nop"; 
          pNop(outputFile);
          break;    
        case 0x9:                                               //"ret";
          pRet(outputFile);
          break;      
      }

    } else {													// read 2nd byte of instruction
        int c1 = fgetc(machineCode);
        int c2a = c1 >> 4;
        int c2b = c1 & 0xf; 
        iCd = c >> 4;
        int cadd = ftell(machineCode) - 1; 

        bool finalC = false; 									//set default value to false
        
        if ((iCd == 2)||(iCd == 4)||(iCd == 5)||(iCd == 6)) { 	// check rA and rB are in the range 0-14 [14 or F] 
          condition3a = (0 <= c2a)&&(c2a <= 14)&&(0 <= c2b)&&(c2b <= 14);
        }

        if (iCd == 3) {  //check if the corresponding register is F as expected
          condition3b = (c2a == 15);
        }

        if ((iCd == 10)||(iCd == 11)) {
          condition3c = (0 <= c2a)&&(c2a <= 14)&&(c2b == 15);
        }

        if((length - cadd) > 8){								//check if there are enough bytes to read
          finalC = true;
        }

        bool InsValidReadSecondByte = condition3a && condition3b && condition3c && finalC;

        if (InsValidReadSecondByte) { // if the second bytes (register numbers) are as expected and there are enough bytes to read
            switch (iCd) {
              case 0x2: 
                if (iFn == 0){         								  //"rrmovq";     					                             
                  pRrmovq(outputFile, c2a, c2b);
                } else {             								  //"cmovq"
                  pCmov(outputFile, iFn, c2a, c2b);
                }
                break; 
              case 0x3:                                               //"irmovq V,rB";
                fread(&v, sizeof(unsigned long), 1, machineCode);
                unsigned long virmovqLittleEndian = BigEndianToLittleEndian(v);
                pIrmovq(outputFile, c2b, v,virmovqLittleEndian);
                break; 
              case 0x4:                                               //"rmmovq rA,D(rB)";
                fread(&d, sizeof(unsigned long), 1, machineCode);
                unsigned long drmmovqLittleEndian = BigEndianToLittleEndian(d);
                pRmmovq(outputFile, c2a, c2b, d, drmmovqLittleEndian);
                break; 
              case 0x5:                                               //"mrmovq D(rB),rA";
                fread(&d, sizeof(unsigned long), 1, machineCode);
                unsigned long dmrmovqLittleEndian = BigEndianToLittleEndian(d);
                pMrmovq(outputFile, c2a, c2b, d, dmrmovqLittleEndian);
                break; 
              case 0x6:                                               //"OPq rA,rB";
                pOpq(outputFile, iFn, c2a, c2b);
                break; 
              case 0x7:                                               //"jXX Dest";
              	fseek(machineCode, -1, SEEK_CUR); 
                fread(&dest, sizeof(unsigned long), 1, machineCode);
                unsigned long destJumpLittleEndian = BigEndianToLittleEndian(dest);
                pJump(outputFile, iFn, dest, destJumpLittleEndian);
                break; 
              case 0x8:                                               //"call Dest";
              	fseek(machineCode, -1, SEEK_CUR); 
                fread(&dest, sizeof(unsigned long), 1, machineCode);
                unsigned long  destCallLittleEndian = BigEndianToLittleEndian(dest);
                pCall(outputFile, dest, destCallLittleEndian);
                break; 
              case 0xa:                                               //"pushq rA";
                pPushq(outputFile, c2a);
                break; 
              case 0xb:                                               //"popq rA";
                pPopq(outputFile, c2a);
                break; 
            }

        } else { // 2nd byte not valid case

              	fseek(machineCode, -1, SEEK_CUR);
        	  	int startingAdd2 = ftell(machineCode) - 1;
  				// check starting address (position) of the would-be instruction is a multiple of 8, 
  				// and there are at least 8 bytes available for reading
			  	if ( ((startingAdd2 % 8) == 0) && ((length - startingAdd2) >= (8*sizeof(char))) ) {
			  		unsigned long number1;
            		fseek(machineCode, -1, SEEK_CUR);
			  		fread(&number1, sizeof(unsigned long), 1, machineCode);
			  		fprintf(outputFile, ".quad 0x%lx\n", number1);
			  	} else {		
			  		fprintf(outputFile, ".byte 0x%2X\n", c);
			  	}
        }
    }

  } else { // 1st byte not valid case

  	int startingAdd1 = ftell(machineCode) - 1;
  	// check starting address (position) of the would-be instruction is a multiple of 8, 
  	// and there are at least 8 bytes available for reading
  	if ( ((startingAdd1 % 8) == 0) && ((length - startingAdd1) >= (8*sizeof(char))) ) {
  		unsigned long number1;
      	fseek(machineCode, -1, SEEK_CUR);
  		fread(&number1, sizeof(unsigned long), 1, machineCode);
  		fprintf(outputFile, ".quad 0x%lx\n", number1);
  	} else {
  		fprintf(outputFile, ".byte 0x%2x\n", c);
  	}

  }
}

  //end of my code.///////////////////////////////////////////////////////////////

  fclose(machineCode);
  fclose(outputFile);
  return SUCCESS;
}







