/*
* File Name: table.h
* Compiler: MS Visual Studio 2019
* Author: Asim Jasarevic
* Course: CST 8152 – Compilers, Lab Section: 013
* Assignment: 2
* Date: 15 Nov 2020
* Professor: Paulo Sousa / Haider Miraj
* Purpose: define custom states and tables to be used in scanner.c when scanning char or lexeme string
*/


/* Macro section */
#ifndef  TABLE_H_
#define  TABLE_H_ 

#ifndef BUFFER_H_
#include "buffer.h"
#endif

#ifndef TOKEN_H_
#include "token.h"
#endif

#ifndef NULL
#include <_null.h> /* NULL pointer constant is defined there */
#endif

 /*  Source end-of-file (SEOF) sentinel symbol
  *    '\0' or one of 255,0xFF,EOF
  */
  /* TODO_02: EOF definitions */
#define CHARSEOF0 0x0000	/*0000 0000 0000 0001 */
#define CHARSEOF255 0x00FF  /*0000 0000 1111 1111*/

/*  Special case tokens processed separately one by one
 *  in the token-driven part of the scanner
 *  '=' , ' ' , '(' , ')' , '{' , '}' , == , <> , '>' , '<' , ';',
 *  white space
 *  !!comment , ',' , ';' , '-' , '+' , '*' , '/', ## ,
 *  _AND_, _OR_, _NOT_ , SEOF,
 */
 /* TODO_03: Error states and illegal state */
#define ES  -1		/* Error state  with no retract */
#define ER  -2		/* Error state  with retract */
#define IS  -3		/* Illegal state */
#define NLL_T 19 /* null token */

#define MAX_INT_P 32767

 /* State transition table definition */
#define TABLE_COLUMNS 8

/* Transition table - type of states defined in separate table */
/* TODO_04: Define Transition table missing values */
int  transitionTable[][TABLE_COLUMNS] = {
	/*				[A-z](0),	0(1),	[1-9](2),	.(3),	$(4),	"(5),	SEOF(6),	other(7)	*/
	/*				L,			Z,		N,			P,		S,		Q,		E,			O			*/
	/* State 00 */  {0,			0,		0,			ES,		ES,		0,		ER,			ES},
	/* State 01 */  {1,			1,		1,			0,		0,		0,		0,			 0},
	/* State 02 */  {IS,		IS,		IS,			IS,		IS,		IS,		IS,			IS},
	/* State 03 */  {IS,		IS,		IS,			IS,		IS,		IS,		IS,			IS},
	/* State 04 */  {ES,		0,		4,			0,		0,		ES,		0,			 0},
	/* State 05 */  {IS,		IS,		IS,			IS,		IS,		IS,		IS,			IS},
	/* State 06 */  {ES,		6,		ES,			0,		0,		ES,		0,			 0},
	/* State 07 */  {0,			7,		7,			7,		0,		0,		0,			 0},
	/* State 08 */  {IS,		IS,		IS,			IS,		IS,		IS,		IS,			IS},
	/* State 09 */  {9,			9,		9,			9,		9,		0,		ER,			 9},
	/* State 10 */  {IS,		IS,		IS,			IS,		IS,		IS,		IS,			IS},
	/* State 11 */  {IS,		IS,		IS,			IS,		IS,		IS,		IS,			IS},
	/* State 12 */  {IS,		IS,		IS,			IS,		IS,		IS,		IS,			IS}
};

/* Accepting state table definition */
/* TODO_05: Define accepting states types */
#define ASWR	13	/* accepting state with retract */
#define ASNR	14	/* accepting state with no retract */
#define NOAS	19	/* not accepting state */

/* List of acceptable states */
/* TODO_06: Define list of acceptable states */
int stateType[] = {
	2,
	3,
	5,
	8,
	10,
};

/* Accepting action function declarations */
/* TODO_07: Declare accepting states functions */
int aStateFuncAVID(char lexeme[]);
int aStateFuncSVID(char lexeme[]);
int aStateFuncIL(char lexeme[]);
int aStateFuncFPL(char lexeme[]);
void aStateFuncSL(char lexeme[]);
int aStateFuncErr(char lexeme[]);
static int isKeyword(char* kw_lexeme);
void checkIfLexemHeld(char* string, char* subString, char c2, char c);


/* Defining a new type: pointer to function (of one char * argument)
   returning Token
*/
typedef Token(*PTR_AAF)(char* lexeme);

/* Accepting function (action) callback table (array) definition */
/* If you do not want to use the typedef, the equvalent declaration is:
 * Token (*finalStateTable[])(char lexeme[]) = {
 */
 /* TODO_08: Define final state table */
PTR_AAF finalStateTable[] = {
	2,
	3,
	5,
	8,
	10,
	11,
	12
};

/* Keyword lookup table (_AND_, _OR_ and _NOT_ are not keywords) */
/* TODO_09: Define the number of Keywords from the language */
#define KWT_SIZE 2

/* Keyword list */
/* TODO_10: Define the list of keywords */
char* keywordTable[] = {
	"INPUT",
	"PROGRAM",
	"DO",
	"WHILE",
	"TRUE",
	"IF",
	"THEN",
	"ELSE",
	"OUTPUT",
	"FALSE"
};

char* tempString;

#endif
