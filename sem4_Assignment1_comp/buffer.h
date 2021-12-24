/*
* IMPORTANT NOTE (2DO):
* RENAME this file as "buffer.h"
* REPLACE the file header with your file header (see CST8152_ASSAMG.pdf for details).
* File Name: buffer.h
* Version: 2.01
* Author: Asim Jasarevic
* Date: 18 Sep 2020
* Preprocessor directives, type declarations and prototypes necessary for buffer implementation
* as required for lab02.
*/

#ifndef BUFFER_H_
#define BUFFER_H_

/*#pragma warning(1:4001) *//*to enforce C89 type comments  - to make //comments an warning */

/*#pragma warning(error:4001)*//* to enforce C89 comments - to make // comments an error */

/* standard header files */
#include <stdio.h>  /* standard input/output */
#include <malloc.h> /* for dynamic memory allocation*/
#include <limits.h> /* implementation-defined data type ranges and limits */

/* constant definitions */
#define RT_FAIL_1 (-1)         /* operation failure return value 1 */
#define RT_FAIL_2 (-2)         /* operation failure return value 2 */
#define LOAD_FAIL (-2)         /* load fail return value */

#define DEFAULT_INIT_CAPACITY 200   /* default initial buffer capacity */
#define DEFAULT_INC_FACTOR 15       /* default increment factor */


/* You should add your own constant definitions here */

/* Add your bit-masks constant definitions here */
#define DEFAULTZ 0x0000			//0000 0000 0000 0000 
#define DEFAULT_FLAGS  0xFFF9 	//1111 1111 1111 1001
#define SET_EOB 0x0001			//0000 0000 0000 0001  
#define RESET_EOB 0xFFFE		//1111 1111 1111 1110
#define CHECK_EOB 0x0001		//0000 0000 0000 0001
#define SET_R_FLAG 0x0002		//0000 0000 0000 0010
#define RESET_R_FLAG 0xFFFD		//1111 1111 1111 1101
#define CHECK_R_FLAG 0x0002		//0000 0000 0000 0010

/* user data type declarations */
/* user data type declarations */
typedef struct BufferEntity {
	char* string;   /* pointer to the beginning of character array (character buffer) */
	short capacity;    /* current dynamic memory size (in bytes) allocated to character buffer */
	short addCPosition;  /* the offset (in chars) to the add-character location */
	short getCPosition;  /* the offset (in chars) to the get-character location */
	short markCPosition; /* the offset (in chars) to the mark location */
	char  increment; /* character array increment factor */
	char  opMode;       /* operational mode indicator*/
	unsigned short flags;     /* contains character array reallocation flag and end-of-buffer flag */
} Buffer, * pBuffer;

/*
IMPORTANT NOTE (2DO):
Place your function declarations here.
Do not include the function header comments here.
Place them in the buffer.c file
*/
char** bufferLoad(pBuffer ptr_Buffer, char* file);
void BufferPrint(pBuffer ptr_Buffer);
pBuffer BufferAddChar(char* string, pBuffer ptr_Buffer);
pBuffer bufferCreate();
char* bufferSetEnd(pBuffer ptr_Buffer, char* endOfFile);
void bufferFree();
int bufferGetCapacity(pBuffer ptr_Buffer);
int bufferAddCPosition(pBuffer ptr_Buffer);
void GetOpMode();
void GetIncrement();
void GetFlags();
void GetRewind();
void GetPrint();

#endif
