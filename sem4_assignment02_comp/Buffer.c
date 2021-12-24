/*
* File Name: Buffer.c
* Compiler: MS Visual Studio 2019
* Author: Asim Jasarevic
* Course: CST 8152 – Compilers, Lab Section: 013
* Assignment: 2
* Date: 15 Nov 2020
* Professor: Paulo Sousa / Haider Miraj
* Purpose: Preprocessor directives, type declarations and prototypes necessary for buffer implementation in assignment 2
*
* Function list: bufferCreate(), bufferAddChar(), bufferFree(), bufferIsFull(), bufferGetAddCPosition(), bufferGetCapacity()
*				 bufferGetOpMode(), bufferGetIncrement(), bufferLoad(), bufferIsEmpty(), bufferGetChar(), bufferPrint(), bufferSetEnd(),
*				 bufferReset(), bufferGetCPosition(), bufferRewind(), bufferGetString(), bufferGetFlags()
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "buffer.h"


/*Function name: bufferCreate
Purpose: declare new struct and assign values based off inputs and values when called
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Called functions: malloc()
Parameters: initCapacity can not :[<0] incFactor can not :[>100] opMode can not :[anyhting else but a,f,m]
Return value: pBuffer p;
Algorithm: Create struct decleration and run through ifs to check  and set struct data*/
pBuffer bufferCreate(short initCapacity, char incFactor, char opMode)
{
	/*error condition if passed values don't hold data can't work
	  also if values hold bad data can't work*/
	if (incFactor < 0 || !opMode || initCapacity < 0 ||
		incFactor > 100 || initCapacity < 0 || initCapacity >= SHRT_MAX)
	{
		return NULL;
	}

	pBuffer p = (pBuffer)calloc(1, sizeof(Buffer));

	if (p) {

		/*set op modes based off opMode passed*/
		if (opMode == 'a') { p->opMode = 1; }
		if (opMode == 'm') { p->opMode = -1; }
		if (opMode == 'f') { opMode = 0; }

		/*set increment and increment as labeled in assignment depending on passed data*/
		if (opMode == 'f') {
			p->increment = 0;
		}
		if (opMode == 'a' && initCapacity > 0) {
			p->increment = incFactor;
			p->opMode = 1;
		}
		if (opMode == 'm' && incFactor >= 1 && incFactor <= 100) {
			p->increment = incFactor;
			p->opMode = -1;
		}
		if (incFactor == 0 && initCapacity != 0) {
			p->increment = 0;
			p->opMode = 0;
		}
		if (opMode == 'a' && incFactor < 0) {
			p->increment = DEFAULT_INC_FACTOR;
		}

		/*if passed initCapacity = 0 set capacity and increment as labeled in assignment*/
		if (initCapacity == 0) {

			p->capacity = DEFAULT_INIT_CAPACITY;

			if (opMode == 'a') { p->increment = DEFAULT_INC_FACTOR; }
			if (opMode == 'm') { p->increment = DEFAULT_INC_FACTOR; }
			if (opMode == 'f') { p->increment = 0; }

		}
		else {
			p->capacity = initCapacity; /* check if this fucks up*/
		}

		/*set addCPosition,flags,string no matter passed data*/
		p->addCPosition = 0;
		p->flags = DEFAULT_FLAGS;
		p->string = (char*)malloc(p->capacity);
		p->initIncrement = incFactor;
	}

	return p;
}

/*Function name: bufferAddChar
Purpose: place single char in *char at passed index, also increase buffer string if allowed
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Called functions: realloc()
Parameters: pBuffer const pBE can not be :[NULL] symbol can not be:[NULL]
Return value: return pBE;
Algorithm: check if allowed to increase capacity depending on op mode capacity is increased differently, after add char to char* at index
		   check if char was able to be placed, return buffer*/
pBuffer bufferAddChar(pBuffer const pBE, char symbol) {

	/*error condition if one of these are not set will cause add char to break */
	if (!pBE)
	{
		return NULL;
	}

	pBuffer p = pBE;
	int newCapacity = pBE->capacity + pBE->increment; /*new capacity after increment*/
	int unableToLoad = pBE->addCPosition; /*current addCPosition before increments or changes*/
	float hundredDiv = 100.0; /*used in opmode 'm' calculation as a divide by 100*/

	pBE->flags &= RESET_R_FLAG;
	int maxSpace = 1; /*to be used so capacity can be lowerd by 1 to prevent lost bit*/

	/*reason for this is so if the increment is 100 or more that casues >99/100 to subtract 1 to the curretn cpacity so - 1 not needed as will casue overflow*/
	if (pBE->increment > 99) {
		maxSpace = 0;
	}

	/*reason for this is so if the increment is 100, 100/100 doesn't return 1 instead equation will now be 100/101*/
	if (pBE->increment > 99) {
		hundredDiv = 101.0;
	}

	/* if opmode m and current position is going to go over capacity increase capacity*/
	if (pBE->opMode == -1 && pBE->addCPosition + 1 > pBE->capacity) {
		//printf("cap%d max%d", pBE->capacity, SHRT_MAX);

		int availableSpace = SHRT_MAX - pBE->capacity - 1; /*use as availableSpace in equation*/
		double div100 = pBE->increment / hundredDiv; /*used for increment / hundredDiv in equation*/

		if (div100 < 1.0 && div100 > 0.0 && pBE->initIncrement > 0) { div100 = 1; }/*this is so once increment * hundredDiv retrun les then 1 wont be stuck in loop not incremnting capacity**/
		int newIncrement = availableSpace * (div100); /*have to div by 101 instead of 100 becasue 100/100 reults in 1 which breaks capacity*/


		pBE->capacity = pBE->capacity + newIncrement;
		p = (char*)realloc(p->string, p->capacity); /*purpose of using p buffer instead of passed pBE is to prevent memory leak and warning*/
		pBE->string = p;
		/*pBE->string = (char*)realloc(pBE->string, pBE->capacity); */
	}

	/* if opmode == a and current position is going to go over capacity increase capacity*/
	if (pBE->opMode == 1 && pBE->addCPosition + 1 > pBE->capacity) {

		pBuffer p = pBE;

		/* if new capacity won't go over max capacity increase capacity*/
		if (newCapacity <= SHRT_MAX - 1) {
			pBE->capacity = pBE->capacity + pBE->increment;
			p->string = (char*)realloc(pBE->string, pBE->capacity);
			pBE->string = p->string;
			/*pBE->string = (char*)realloc(pBE->string, pBE->capacity); */
		}
	}

	/* if no need to increase capacity add given char to Cposition */
	if (pBE->addCPosition + 1 <= pBE->capacity) {

		/* to stop the potential of dereferencing NULL in pointer->string*/
		if (pBE->string) {
			pBE->string[pBE->addCPosition] = symbol;
		}

		pBE->addCPosition++;

		/* to stop the potential of dereferencing NULL in pointer->string*/
		if (pBE->string) {
			pBE->string[pBE->addCPosition] = '/0'; /* prevent garbage data to show when printing empty file*/
		}
	}

	/* if addCPosition was not able to incremnt after all those if's then we have reached end of buffer and can not add char */
	if (unableToLoad == pBE->addCPosition) {
		return NULL;
	}

	return pBE;
}

/*Function name: bufferFree
Purpose: frees memory of buffer
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Called functions: free()
Parameters: pBuffer const pBE can not be :[NULL]
Return value: void
Algorithm: N/A*/
void bufferFree(Buffer* const pBE) {

	/*check if buffer exsists to prevent freeing nothing*/
	if (pBE && pBE->string) {
		free(pBE->string);
		free(pBE);
	}

}

/*Function name: bufferIsFull
Purpose: check if buffer is full
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Called functions: strlen()
Parameters: pBuffer const pBE can not be :[NULL]
Return value: return int
Algorithm: N/A*/
int bufferIsFull(Buffer* const pBE) {

	/*check if buffer at string length - 1 is occupied if occupied not end of buffer */
	if (pBE->string[strlen(pBE->string)] != NULL) {
		return 1;
	}
	return 0;
}

/*Function name: bufferGetAddCPosition
Purpose: get current AddCPosition from buffer
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Called functions: N/A
Parameters: pBuffer const pBE can not be :[NULL]
Return value: return pBE->addCPosition;
Algorithm: N/A*/
short bufferGetAddCPosition(Buffer* const pBE) {

	return pBE->addCPosition;
}

/*Function name: bufferGetCapacity
Purpose: get current Capacity from buffer
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Called functions: N/A
Parameters: pBuffer const pBE can not be :[NULL]
Return value: return pBE->Capacity;
Algorithm: N/A*/
short bufferGetCapacity(Buffer* const pBE) {

	return pBE->capacity;
}

/*Function name: bufferGetOpMode
Purpose: get current OpMode from buffer
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Called functions: N/A
Parameters: pBuffer const pBE can not be :[NULL]
Return value: return pBE->OpMode;
Algorithm: N/A*/
int bufferGetOpMode(Buffer* const pBE) {

	return pBE->opMode;
}

/*Function name: bufferGetincrement
Purpose: get current increment from buffer
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Called functions: N/A
Parameters: pBuffer const pBE can not be :[NULL]
Return value: return pBE->increment;
Algorithm: N/A*/
size_t bufferGetIncrement(Buffer* const pBE) {

	return pBE->increment;
}

/*Function name: bufferLoad
Purpose: get all postions from given file and call function to get char from file
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Called functions: fgetc(), feof(), bufferAddChar(), ungetc()
Parameters: FILE* const fi can not be :[non existent], pBuffer const pBE can not be :[NULL]
Return value: return elementsLoaded;
Algorithm: while loop checks if end of file is reached, if not tries to call function to add char to buffer string, if function called without
		   problem increments int var and returns it as the current location of file space*/
int bufferLoad(FILE* const fi, Buffer* const pBE) {
	char c = (char)fgetc(fi); /* char that will hold char in file when read*/
	int elementsLoaded = 0;  /* counter for Cposition*/

	/* while not end of file try to call function to add char
	   if can call c = next char in file and increment position
	   if not called the return fail*/
	while (!feof(fi)) {
		if (!bufferAddChar(pBE, c)) {
			ungetc(c, fi);
			return LOAD_FAIL;
		}
		c = (char)fgetc(fi);
		elementsLoaded++;
	}

	return elementsLoaded;
}

/*Function name: bufferIsEmpty
Purpose: checks if buffer is empty
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Called functions: N/A
Parameters: FILE* pBuffer const pBE can not be :[NULL]
Return value: return int;
Algorithm: N/A*/
int bufferIsEmpty(Buffer* const pBE) {

	if (pBE->addCPosition == 0) {
		return 1;
	}
	return 0;
}

/*Function name: bufferGetChar
Purpose: checks if end of string is reched and gets char at given index
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Called functions: N/A
Parameters: FILE* pBuffer const pBE can not be :[NULL]
Return value: return tempChar or generic int;
Algorithm: check if end of string is reched update flag according to result, stores char of buffer before incrementing getCPosition*/
char bufferGetChar(Buffer* const pBE) {
	char tempChar; /* hold char at getCPosition before increment*/

	pBE->flags |= RESET_R_FLAG;

	/*when getCPosition reaches addCPosition then end of file reached
	  when reached |= flags with r_flag to set last bit to 1*/
	if (pBE->getCPosition == pBE->addCPosition) {
		pBE->flags |= SET_EOB;
		return 0;
	}
	else {
		pBE->flags &= RESET_EOB;
	}

	tempChar = pBE->string[pBE->getCPosition];
	pBE->getCPosition++;
	return tempChar;
}

char bufferGetNextChar(Buffer* const pBE) {
	char tempChar; /* hold char at getCPosition before increment*/

	tempChar = pBE->string[pBE->getCPosition];

	return tempChar;
}

/*Function name: bufferPrint
Purpose: calls function to print chars and rewinds buffer
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Called functions: bufferGetChar(), bufferRewind()
Parameters: FILE* pBuffer const pBE can not be :[NULL] char newLine can not be :[NULL]
Return value: return 0;
Algorithm: for loop calls function which displays char each time called, once done buffer rewind called to reset some buffer values*/
int bufferPrint(Buffer* const pBE, char newLine) {

	int numOfPrints = 0;
	pBE->flags = DEFAULT_FLAGS;

	/*Prints char at index until flag is set to 0xFFFD where then it stops*/
	while (pBE->flags != RESET_R_FLAG) {
		printf("%c", bufferGetChar(pBE));
		numOfPrints++;
	}

	bufferRewind(pBE);

	return numOfPrints;
}

/*Function name: bufferSetEnd
Purpose: set end char of *char to symbol
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Called functions: realloc()
Parameters: FILE* pBuffer const pBE can not be :[NULL] char newLine can not be :[NULL]
Return value: return pBE;
Algorithm: N/A */
Buffer* bufferSetEnd(Buffer* const pBE, char symbol) {

	pBuffer p = pBE;

	/*when called finds last char in string increments by 1 reallocs string adds 'symbol' to last position and sets r_flag*/
	pBE->capacity = pBE->addCPosition + 1;
	p->string = (char*)realloc(pBE->string, pBE->capacity);
	pBE->string = p->string;
	//*(pBE->string + pBE->addCPosition++) = symbol;
	if (pBE->string) {
		pBE->string[pBE->addCPosition] = symbol;
	}
	pBE->addCPosition++;
	pBE->flags |= SET_EOB;

	return pBE;
}

/*Function name: bufferReset
Purpose: set getCPosition to markCPosition
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Called functions: N/A
Parameters: FILE* pBuffer const pBE can not be :[NULL]
Return value: return pBE->getCPosition;
Algorithm: N/A */
short bufferReset(Buffer* const pBE) {

	pBE->getCPosition = pBE->markCPosition;

	return pBE->getCPosition;
}

/*Function name: bufferGetCPosition
Purpose: get getCPosition from buffer
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Called functions: N/A
Parameters: FILE* pBuffer const pBE can not be :[NULL]
Return value: return pBE->getCPosition;
Algorithm: N/A */
short bufferGetCPosition(Buffer* const pBE) {

	return pBE->getCPosition;
}

/*Function name: bufferRewind
Purpose: set getCPosition & markCPosition back to 0
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Called functions: N/A
Parameters: FILE* pBuffer const pBE can not be :[NULL]
Return value: return 0;
Algorithm: N/A */
int bufferRewind(Buffer* const pBE) {

	/*check if buffer getCPosition exsists if not can't get value
	check if buffer exsists at all*/
	if (!pBE || pBE->getCPosition < 0 || pBE->getCPosition >= SHRT_MAX) {
		return -1;
	}

	pBE->getCPosition = 0;
	pBE->markCPosition = 0;

	return 0;
}

/*Function name: bufferGetString
Purpose: get string from buffer at index charPosition
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Called functions: N/A
Parameters: FILE* pBuffer const pBE can not be :[NULL] short charPosition can not be :[NULL]
Return value: return pBE->string[charPosition]
Algorithm: N/A */
char* bufferGetString(Buffer* const pBE, short charPosition) {

	return pBE->string + charPosition;
}

/*Function name: bufferGetString
Purpose: get flag from buffer
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Called functions: N/A
Parameters: FILE* pBuffer const pBE can not be :[NULL]
Return value: return pBE->flags
Algorithm: N/A */
unsigned short bufferGetFlags(pBuffer const pBE) {

	return pBE->flags;
}