/*
* File Name: Buffer.h
* Compiler: MS Visual Studio 2019
* Author: Asim Jasarevic
* Course: CST 8152 – Compilers, Lab Section: 013
* Assignment: 1
* Date: 3 Oct 2020
* Professor: Paulo Sousa / Haider Miraj
* Purpose: Preprocessor directives, type declarations and prototypes necessary for buffer implementation in assignment 1
* 
* Function list: bufferCreate(), bufferAddChar(), bufferClear(), bufferFree(), bufferIsFull(), bufferGetAddCPosition(), bufferGetCapacity()
*				 bufferGetOpMode(), bufferGetIncrement(), bufferLoad(), bufferIsEmpty(), bufferGetChar(), bufferSetEnd(), bufferRetract()
*				 bufferReset(), bufferGetCPosition(), bufferRewind(), bufferGetString(), bufferSetMarkPosition(), bufferGetFlags()
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
#define DEFAULT_FLAGS 0xFFFC 	/*1111 1111 1111 1100 */
#define SET_EOB 0x0001			/*0000 0000 0000 0001 */
#define RESET_EOB 0xFFFE		/*1111 1111 1111 1110 */
#define CHECK_EOB 0x0001		/*0000 0000 0000 0001 */
#define SET_R_FLAG 0x0002		/*0000 0000 0000 0010 */
#define RESET_R_FLAG 0xFFFD		/*1111 1111 1111 1101 */
#define CHECK_R_FLAG 0x0002		/*0000 0000 0000 0010 */


/* user data type declarations */
/* user data type declarations */
typedef struct BufferEntity {
	char* string;   /* pointer to the beginning of character array (character buffer) */
	short capacity;    /* current dynamic memory size (in bytes) allocated to character buffer */
	int addCPosition;  /* the offset (in chars) to the add-character location */
	short getCPosition;  /* the offset (in chars) to the get-character location */
	short markCPosition; /* the offset (in chars) to the mark location */
	char  increment; /* character array increment factor */
	char  initIncrement; /* character array initial increment factor */
	char  opMode;       /* operational mode indicator*/
	unsigned short flags;     /* contains character array reallocation flag and end-of-buffer flag */
} Buffer, * pBuffer;

/*
IMPORTANT NOTE (2DO):
Place your function declarations here.
Do not include the function header comments here.
Place them in the buffer.c file
*/
pBuffer bufferCreate(short initCapacity, char incFactor, char opMode); /*create buffer with a size limit*/
pBuffer bufferAddChar(pBuffer const pBE, char symbol);
void bufferFree(Buffer* const pBE);
int bufferIsFull(Buffer* const pBE);
short bufferGetAddCPosition(Buffer* const pBE);
short bufferGetCapacity(Buffer* const pBE);
int bufferGetOpMode(Buffer* const pBE);
size_t bufferGetIncrement(Buffer* const pBE);
int bufferLoad(FILE* const fi, Buffer* const pBE);
int bufferIsEmpty(Buffer* const pBE);
char bufferGetChar(Buffer* const pBE);
Buffer* bufferSetEnd(Buffer* const pBE, char symbol);
short bufferReset(Buffer* const pBE);
short bufferGetCPosition(Buffer* const pBE);
int bufferRewind(Buffer* const pBE);
char* bufferGetString(Buffer* const pBE, short charPosition);
unsigned short bufferGetFlags(pBuffer const pBE);

#endif
