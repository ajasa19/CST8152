/*
* File Name: scanner.c
* Compiler: MS Visual Studio 2019
* Author: Asim Jasarevic
* Course: CST 8152 – Compilers, Lab Section: 013
* Assignment: 2
* Date: 15 Nov 2020
* Professor: Paulo Sousa / Haider Miraj
* Purpose: take in char at buffer and through steps decide what state should be given to current token
*
* Function list: initScanner(), processToken(), getNextState(), nextTokenClass(), aStateFuncAVID(), aStateFuncSVID(), aStateFuncIL()
*				 aStateFuncFPL(), aStateFuncSL(), aStateFuncErr(), isKeyword(), checkIfLexemHeld()
*/

/* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
 * to suppress the warnings about using "unsafe" functions like fopen()
 * and standard sting library functions defined in string.h.
 * The define does not have any effect in Borland compiler projects.
 */
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */
#include <float.h>   /* floating-point types constants */

 /*#define NDEBUG        to suppress assert() call */
#include <assert.h>  /* assert() prototype */

/* project header files */

#include "token.h"
#include "table.h"

#define DEBUG  /* for conditional processing */
#undef  DEBUG

/* Global objects - variables */
/* This buffer is used as a repository for string literals.
   It is defined in platy_st.c */
extern pBuffer stringLiteralTable;		/* String literal table */
int line;								/* current line number of the source code */
extern int errorNumber;					/* defined in platy_st.c - run-time error number */

static char debugMode = 0;				/* optional for debugging */

/* Local(file) global objects - variables */
static pBuffer lexemeBuffer;			/* pointer to temporary lexeme buffer */
static pBuffer sourceBuffer;			/* pointer to input source buffer */
/* No other global variable declarations/definitiond are allowed */

/* scanner.c static(local) function  prototypes */
static int nextTokenClass(char c);		/* character class function */
static int getNextState(int, char);		/* state machine function */
static int isKeyword(char* kw_lexeme);	/* keywords lookup functuion */

int sizeofString = 0;				/*used for STR_T for how large the string is*/
int totalSizeofLexemTable = 0;		/*the overall size of STR_T in buffer*/

int charPoint = 0;					/*index of main string*/
int charPointLexem = 0;				/*index of main Lexeme string*/
int charPointSubString = 0;			/*index of main sub string*/
int charPointStr = 0;				/*index of main STR_T string*/
int charPointErr = 0;				/*index of main coment error string*/
int charErrCalled = 0;				/*used as bool to see if a comment error was found*/
int quoteCounter = 0;				/*counter to see if quote has been found two times to close a STR_T*/
int startLexemCheck = 0;			/**used as bool to see if Lexeme string should now be used to check for state*/


char string[155];					/*string which holds entire line of buffer*/
char subString[155];				/*subString used to find LOG_OP_T*/
char stringLexem[155];				/*stringLexem used to hold valid states untill non-valid character hit*/
char stringStr[250];				/*stringStr used to hold quote strings (STR_T)*/
char stringErr[200];				/*stringErr used to hold comment errors*/
char charErr;						/*charErr used to hold char which caused ERR_T to be called*/

/* TODO_02: Follow the standard and adjust all function headers */

/*Function name: initScanner
Purpose: did not use in implementation
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Called functions: bufferRewind(), bufferIsEmpty
Parameters: pBuffer psc_buf
Return value: EXIT_SUCCESS;
Algorithm: not used*/
int initScanner(pBuffer psc_buf) {
	if (bufferIsEmpty(psc_buf)) return EXIT_FAILURE;/*1*/
	/* in case the buffer has been read previously  */
	bufferRewind(psc_buf);
	line = 1;
	sourceBuffer = psc_buf;
	return EXIT_SUCCESS;/*0*/
}


/*Function name: processToken
Purpose: load char of buffer and handels if any strings/current char should call a function or set a state.
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Called functions: bufferGetChar(sourceBuffer), bufferGetNextChar(sourceBuffer), aStateFuncAVID(), aStateFuncSVID(), aStateFuncIL(), aStateFuncFPL(), aStateFuncSL(), aStateFuncErr(), isKeyword(), checkIfLexemHeld()
Return value: currentToken;
Algorithm: add current char to many strings loop through if's and functions to see if any state can be found. if state found set tooken values if not return null state*/
Token processToken(void) {
	Token currentToken = { 0 }; /* token to return after pattern recognition. Set all structure members to 0 */
	unsigned char c;	/* input symbol */
	unsigned char c2;
	int state = 0;		/* initial state of the FSM */
	short lexStart;		/* start offset of a lexeme in the input char buffer (array) */
	short lexEnd;		/* end offset of a lexeme in the input char buffer (array)*/

	int lexLength;		/* token length */
	int i;				/* counter */
	int isError;		/* error identification */
	unsigned char newc;	/* new char */

	while (1) { /* endless loop broken by token returns it will generate a warning */

		/*if charErrCalled was set because aStateFuncErr was called then this token should be set to ERR_T*/
		if (charErrCalled == 1) {

			currentToken.code = ERR_T; /*set token to error state*/
			currentToken.attribute.err_lex[0] = charErr; /*set token err lex to charErr*/

			/*set index, strings, and vars to 0/empty*/
			charErrCalled = 0;
			charPointLexem = 0;
			memset(stringLexem, 0, sizeof stringLexem);
			startLexemCheck = 0;

			return currentToken;

		}

		c = bufferGetChar(sourceBuffer);		/*current char in buffer*/
		c2 = bufferGetNextChar(sourceBuffer);	/*next char in buffer*/

		/*if the string is going to overflow or EOF was set this will set token to EOF*/
		if (c == EOF || charPointLexem > 154 || charPointStr > 154) {
			currentToken.code = SEOF_T;
			return currentToken;
		}

		/*if to check if a comment error is prevelent so if only one ! exsist and there is content after it call error*/
		if (strstr(stringErr, "!!") == NULL && strstr(stringErr, "!") != NULL && c == '\n' || strstr(stringErr, "!!") == NULL && strstr(stringErr, "!") != NULL && charPointErr > 149) {

			/*make sure this is a comment error by calling aStateFuncErr*/
			if (aStateFuncErr(stringErr) == 5) {

				/*set token to ERR_T and set err_lex*/
				currentToken.code = ERR_T;
				strcpy(currentToken.attribute.err_lex, stringErr);

				/*set index, strings, and vars to 0/empty*/
				charPointErr = 0;
				memset(stringErr, 0, sizeof stringErr);
				charPoint = 0;
				charPointSubString = 0;
				charPointErr = 0;
				startLexemCheck = 0;
				memset(subString, 0, sizeof subString);
				memset(string, 0, sizeof string);
				memset(stringErr, 0, sizeof stringErr);

				return currentToken;
			}
		}

		/*if it is not a newline add char to some strings*/
		if (c != '\n') {

			/*adding char to strings*/
			string[charPoint] = c;
			subString[charPointSubString] = c;
			stringErr[charPointErr] = c;

			/*incrementing indexs*/
			charPoint++;
			charPointSubString++;
			charPointErr++;

		}
		else {	/*if it is a new line reset strings and indexs*/

			/*clearing strings*/
			memset(subString, 0, sizeof subString);
			memset(string, 0, sizeof string);
			memset(stringErr, 0, sizeof stringErr);
			line++;

			/*reseting indexs*/
			charPoint = 0;
			charPointSubString = 0;
			charPointErr = 0;
			startLexemCheck = 0;
		}

		/*if there is a valid comment in overall string disregard evreything after !! and keep clearing strings until comment is passed*/
		if (strstr(string, "!!") != NULL) {

			/*clearing strings*/
			memset(subString, 0, sizeof subString);
			memset(stringLexem, 0, sizeof stringLexem);
			memset(stringStr, 0, sizeof stringStr);

			/*reseting indexs*/
			charPointSubString = 0;
			charPointLexem = 0;
			charPointStr = 0;
		}

		/*if char is " or a string has already started add chars to stringStr or start state checking*/
		if (c == '"' && !strstr(string, "!!") || charPointStr > 0 && !strstr(string, "!!")) {

			/*if a " already exsist and another qoute is found or stringStr is going to overflow run state check*/
			if (c == '"' && charPointStr > 0 || charPointStr > 148) {

				/*if was called due to string overflow, error occured as no second " was ever placed*/
				if (charPointStr > 148) {
					/*set token to error and err_lex*/
					currentToken.code = ERR_T;
					strncpy(currentToken.attribute.err_lex, stringStr + 0, 19 - 0);

					/*reseting indexs and var values*/
					charPointStr = 0;
					memset(stringStr, 0, sizeof stringStr);
					sizeofString = 0;
					return currentToken;
				}

				/*if second " was placed run aStateFuncSL to get size of string*/
				aStateFuncSL(stringStr);

				/*set token to STR_T and string*/
				currentToken.code = STR_T;
				strcpy(stringLiteralTable->string, stringStr);

				/*set the size of STR_T token and overall STR_T buffer size*/
				currentToken.attribute.get_int = totalSizeofLexemTable;
				totalSizeofLexemTable = totalSizeofLexemTable + sizeofString + 1;

				charPointStr = 0;
				memset(stringStr, 0, sizeof stringStr);
				sizeofString = 0;
				return currentToken;
			}

			/*gets rid of " at start for stringStr and add char to stringStr*/
			if (charPointStr > 0) {
				stringStr[charPointStr - 1] = c;
			}

			charPointStr++;
			currentToken.code = NOAS;
			return currentToken;
		}

		/*if char is { token is LBR_T
		  reset strings and indexs*/
		if (c == '{' && !strstr(string, "!!")) {
			currentToken.code = LBR_T;

			charPointSubString = 0;
			memset(subString, 0, sizeof subString);
			charPointErr = 0;
			memset(stringErr, 0, sizeof stringErr);
			return currentToken;
		}

		/*if char is } token is RBR_T
		 reset strings and indexs*/
		else if (c == '}' && !strstr(string, "!!")) {
			currentToken.code = RBR_T;

			charPointSubString = 0;
			memset(subString, 0, sizeof subString);
			charPointErr = 0;
			memset(stringErr, 0, sizeof stringErr);
			return currentToken;
		}

		/*if char is , token is COM_T
		 reset strings and indexs*/
		else if (c == ',' && !strstr(string, "!!")) {
			currentToken.code = COM_T;

			charPointSubString = 0;
			memset(subString, 0, sizeof subString);
			charPointErr = 0;
			memset(stringErr, 0, sizeof stringErr);
			return currentToken;
		}

		/*if char is $ or # check for match*/
		else if (c == '$' && !strstr(string, "!!") || c == '#' && !strstr(string, "!!")) {
			/*if subString contains "$$" or "##" token is SCC_OP_T*/
			if (strstr(subString, "$$") || strstr(subString, "##")) {
				currentToken.code = SCC_OP_T;

				charPointSubString = 0;
				memset(subString, 0, sizeof subString);
				charPointErr = 0;
				memset(stringErr, 0, sizeof stringErr);
				return currentToken;
			}
		}

		/*if char is _ check for match*/
		else if (c == '_' && !strstr(string, "!!")) {
			/*if past chars are _AND LOG_OP_T 0 found set token to LOG_OP_T 0 and reset strings*/
			if (strstr(subString, "_AND") && !strstr(string, "!!")) {
				currentToken.attribute.get_int = 0;
				currentToken.code = LOG_OP_T;

				charPointSubString = 0;
				memset(subString, 0, sizeof subString);
				charPointLexem = 0;
				memset(stringLexem, 0, sizeof stringLexem);
				startLexemCheck = 0;
				charPointErr = 0;
				memset(stringErr, 0, sizeof stringErr);
				return currentToken;
			}
			/*if past chars are _OR LOG_OP_T 1 found set token to LOG_OP_T 1 and reset strings*/
			else if (strstr(subString, "_OR") && !strstr(string, "!!")) {
				currentToken.attribute.get_int = 1;
				currentToken.code = LOG_OP_T;

				charPointSubString = 0;
				memset(subString, 0, sizeof subString);
				charPointLexem = 0;
				memset(stringLexem, 0, sizeof stringLexem);
				startLexemCheck = 0;
				charPointErr = 0;
				memset(stringErr, 0, sizeof stringErr);
				return currentToken;
			}
			/*if past chars are _NOT LOG_OP_T 2 found set token to LOG_OP_T 2 and reset strings*/
			else if (strstr(subString, "_NOT") && !strstr(string, "!!")) {
				currentToken.attribute.get_int = 2;
				currentToken.code = LOG_OP_T;

				charPointSubString = 0;
				memset(subString, 0, sizeof subString);
				charPointLexem = 0;
				memset(stringLexem, 0, sizeof stringLexem);
				startLexemCheck = 0;
				charPointErr = 0;
				memset(stringErr, 0, sizeof stringErr);
				return currentToken;
			}
		}

		/*if char is = or < or > check for match*/
		else if (c == '=' && !strstr(string, "!") || c == '<' && !strstr(string, "!") || c == '>' && !strstr(string, "!")) {
			/*if string now holds == set token to REL_OP_T 0 and reset strings*/
			if (strstr(subString, "==") != NULL && !strstr(string, "!")) {
				currentToken.attribute.get_int = 0;
				currentToken.code = REL_OP_T;

				charPointSubString = 0;
				memset(subString, 0, sizeof subString);
				charPointLexem = 0;
				memset(stringLexem, 0, sizeof stringLexem);
				startLexemCheck = 0;
				charPointErr = 0;
				memset(stringErr, 0, sizeof stringErr);
				return currentToken;
			}
			/*if string now holds <> set token to REL_OP_T 1 and reset strings*/
			else if (strstr(subString, "<>") && !strstr(string, "!") != NULL) {
				currentToken.attribute.get_int = 1;
				currentToken.code = REL_OP_T;

				charPointSubString = 0;
				memset(subString, 0, sizeof subString);
				charPointLexem = 0;
				memset(stringLexem, 0, sizeof stringLexem);
				startLexemCheck = 0;
				charPointErr = 0;
				memset(stringErr, 0, sizeof stringErr);
				return currentToken;
			}
			/*if string now holds > but not <> set token to REL_OP_T 2 and reset strings*/
			else if (c == '>' && !strstr(string, "!")) {
				currentToken.attribute.get_int = 2;
				currentToken.code = REL_OP_T;

				charPointSubString = 0;
				memset(subString, 0, sizeof subString);
				charPointLexem = 0;
				memset(stringLexem, 0, sizeof stringLexem);
				startLexemCheck = 0;
				charPointErr = 0;
				memset(stringErr, 0, sizeof stringErr);
				return currentToken;
			}
			/*if string now holds < and next char won't complete <> set token to REL_OP_T 3 and reset strings*/
			else if (c == '<' && c2 != '>' && !strstr(string, "!")) {
				currentToken.attribute.get_int = 3;
				currentToken.code = REL_OP_T;

				charPointSubString = 0;
				memset(subString, 0, sizeof subString);
				charPointLexem = 0;
				memset(stringLexem, 0, sizeof stringLexem);
				startLexemCheck = 0;
				charPointErr = 0;
				memset(stringErr, 0, sizeof stringErr);
				return currentToken;
			}
			/*if string now holds = and not == set token to ASS_OP_T and reset strings*/
			else if (c == '=' && !strstr(string, "!") && c2 != '=') {
				currentToken.code = ASS_OP_T;

				charPointSubString = 0;
				memset(subString, 0, sizeof subString);
				charPointLexem = 0;
				memset(stringLexem, 0, sizeof stringLexem);
				startLexemCheck = 0;
				charPointErr = 0;
				memset(stringErr, 0, sizeof stringErr);
				return currentToken;
			}
			/*if string now holds < and next string will complete <> end token here to prevent invalid char to be put into Lexeme string*/
			else if (c == '<' && c2 == '>' && !strstr(string, "!")) {
				currentToken.code = NOAS;
				return currentToken;
			}
		}

		/*if char is ( token is LPR_T
		reset strings and indexs*/
		else if (c == '(' && !strstr(string, "!!")) {
			currentToken.code = LPR_T;

			charPointSubString = 0;
			memset(subString, 0, sizeof subString);
			charPointErr = 0;
			memset(stringErr, 0, sizeof stringErr);
			return currentToken;
		}

		/*if char is ) token is RPR_T
		reset strings and indexs*/
		else if (c == ')' && !strstr(string, "!!")) {
			currentToken.code = RPR_T;

			charPointSubString = 0;
			memset(subString, 0, sizeof subString);
			charPointErr = 0;
			memset(stringErr, 0, sizeof stringErr);
			return currentToken;
		}

		/*if char is + or - or * or / check for match*/
		else if (c == '+' || c == '-' || c == '*' || c == '/') {
			/*if char is + set token to ART_OP_T 0 and reset string*/
			if (c == '+' && !strstr(string, "!!")) {
				currentToken.attribute.get_int = 0;
				currentToken.code = ART_OP_T;

				charPointSubString = 0;
				memset(subString, 0, sizeof subString);
				charPointErr = 0;
				memset(stringErr, 0, sizeof stringErr);
				return currentToken;
			}
			/*if char is - set token to ART_OP_T 1 and reset string*/
			else if (c == '-' && !strstr(string, "!!")) {
				currentToken.attribute.get_int = 1;
				currentToken.code = ART_OP_T;

				charPointSubString = 0;
				memset(subString, 0, sizeof subString);
				charPointErr = 0;
				memset(stringErr, 0, sizeof stringErr);
				return currentToken;
			}
			/*if char is * set token to ART_OP_T 2 and reset string*/
			else if (c == '*' && !strstr(string, "!!")) {
				currentToken.attribute.get_int = 2;
				currentToken.code = ART_OP_T;

				charPointSubString = 0;
				memset(subString, 0, sizeof subString);
				charPointErr = 0;
				memset(stringErr, 0, sizeof stringErr);
				return currentToken;
			}
			/*if char is / set token to ART_OP_T 3 and reset string*/
			else if (c == '/' && !strstr(string, "!!")) {
				currentToken.attribute.get_int = 3;
				currentToken.code = ART_OP_T;

				charPointSubString = 0;
				memset(subString, 0, sizeof subString);
				charPointErr = 0;
				memset(stringErr, 0, sizeof stringErr);
				return currentToken;
			}
		}

		/*if char is ; token is EOS_T
		reset strings and indexs*/
		else if (c == ';' && !strstr(string, "!!")) {
			currentToken.code = EOS_T;

			charPointSubString = 0;
			memset(subString, 0, sizeof subString);
			charPointErr = 0;
			memset(stringErr, 0, sizeof stringErr);
			return currentToken;
		}





		/*if char is not some form of whitespace and not a comment and he current char doesnt launch startLexemCheck add char to stringLexem*/
		if (c != ' ' && c != '\n' && c != '\v' && c != '\t' && !strstr(string, "!") && startLexemCheck == 0) {
			stringLexem[charPointLexem] = c;
			charPointLexem++;
		}

		/*call function to see if next char or current char will casue Lexeme to reset*/
		checkIfLexemHeld(string, subString, c2, c);

		/*if error relating to int of float where odd char is casuing issue (ex: 10a, a0) stop process let top of while handle issue*/
		if (startLexemCheck == 1 || charPointStr > 148) {

			/*if error relating to int of float basicaly stop process*/
			if (aStateFuncErr(stringLexem) == 6) {

			}

			/*if error relating to int being to big or small return error*/
			else if (aStateFuncErr(stringLexem) == 2) {
				currentToken.code = ERR_T;
				strncpy(currentToken.attribute.err_lex, stringLexem + 0, 19 - 0);

				charPointLexem = 0;
				memset(stringLexem, 0, sizeof stringLexem);
				startLexemCheck = 0;

				return currentToken;
			}

			/*if error relating to float being to big or small return error*/
			else if (aStateFuncErr(stringLexem) == 1) {
				currentToken.code = ERR_T;
				strncpy(currentToken.attribute.err_lex, stringLexem + 0, 19 - 0);

				charPointLexem = 0;
				memset(stringLexem, 0, sizeof stringLexem);
				startLexemCheck = 0;

				return currentToken;
			}

			/*if error relating to string with quotes return error*/
			else if (aStateFuncErr(stringLexem) == 4) {

				currentToken.code = ERR_T;
				strcpy(currentToken.attribute.err_lex, stringLexem);

				charPointLexem = 0;
				memset(stringLexem, 0, sizeof stringLexem);
				startLexemCheck = 0;

				return currentToken;

			}

			/*if error relating to _AND_/_OR_/_NOT_*/
			else if (aStateFuncErr(stringLexem) == 3) {

				currentToken.code = ERR_T;
				strcpy(currentToken.attribute.err_lex, stringLexem);

				charPointLexem = 0;
				memset(stringLexem, 0, sizeof stringLexem);
				startLexemCheck = 0;

				return currentToken;

			}

			/*if no errors are returnd we can now set a valid state by running through state methods*/

		/*if aStateFuncFPL returns 1 set token to FPL_T and set flt_value*/
			if (aStateFuncFPL(stringLexem) == 1) {
				currentToken.code = FPL_T;
				currentToken.attribute.flt_value = atof(stringLexem);

				charPointLexem = 0;
				memset(stringLexem, 0, sizeof stringLexem);
				startLexemCheck = 0;

				return currentToken;
			}

			/*if aStateFuncIL returns 1 set token to INL_T and set get_int*/
			else if (aStateFuncIL(stringLexem) == 1) {
				currentToken.code = INL_T;
				currentToken.attribute.get_int = atoi(stringLexem);

				charPointLexem = 0;
				memset(stringLexem, 0, sizeof stringLexem);
				startLexemCheck = 0;

				return currentToken;
			}

			/*if aStateFuncSVID returns 1 set token to SVID_T and set vid_lex*/
			else if (aStateFuncSVID(stringLexem) == 1) {
				currentToken.code = SVID_T;
				strcpy(currentToken.attribute.vid_lex, stringLexem);

				charPointLexem = 0;
				memset(stringLexem, 0, sizeof stringLexem);
				startLexemCheck = 0;

				return currentToken;
			}

			/*if isKeyword returns 1 set token to KW_T and set get_int*/
			else if (isKeyword(stringLexem) != -1) {
				currentToken.code = KW_T;
				currentToken.attribute.get_int = isKeyword(stringLexem); /*call isKeyword to find proper int*/

				charPointLexem = 0;
				memset(stringLexem, 0, sizeof stringLexem);
				startLexemCheck = 0;

				return currentToken;
			}

			/*if aStateFuncAVID returns 1 set token to AVID_T and set vid_lex*/
			else if (aStateFuncAVID(stringLexem) == 1) {
				currentToken.code = AVID_T;
				strcpy(currentToken.attribute.vid_lex, stringLexem);

				charPointLexem = 0;
				memset(stringLexem, 0, sizeof stringLexem);
				startLexemCheck = 0;

				return currentToken;
			}

			/*even if no state is set and no error is found still reset string to avoid more errors down the line*/
			charPointLexem = 0;
			memset(stringLexem, 0, sizeof stringLexem);
			startLexemCheck = 0;

		}

		/*if nothing at all is called and the token is never set and returned the current char is not acceptable*/
		currentToken.code = NOAS;
		return currentToken;

	} /*while*/

} /*processToken*/


/* DO NOT MODIFY THE CODE OF THIS FUNCTION
YOU CAN REMOVE THE COMMENTS ONLY */
/*************************************************************
 * Get Next State
 ************************************************************/

 /*Function name: getNextState
 Purpose: I was confused by this function from what I gather I assume to get the next state from 2d array. Tried asking prof but never used it due to confusion
 Author: Paulo Sousa
 History/Versions: 1.0, 15 Nov 2020
 Called functions: nextTokenClass(c)
 Return value: currentToken;
*/
int getNextState(int state, char c) {
	int col;
	int next;
	if (debugMode)
		fprintf(stderr, "[03] getNextState\n");
	col = nextTokenClass(c);
	next = transitionTable[state][col];
#ifdef DEBUG
	printf("Input symbol: %c Row: %d Column: %d Next: %d \n", c, state, col, next);
#endif
	/*
	The assert(int test) macro can be used to add run-time diagnostic to programs
	and to "defend" from producing unexpected results.
	assert() is a macro that expands to an if statement;
	if test evaluates to false (zero) , assert aborts the program
	(by calling abort()) and sends the following message on stderr:

	Assertion failed: test, file filename, line linenum

	The filename and linenum listed in the message are the source file name
	and line number where the assert macro appears.
	If you place the #define NDEBUG directive ("no debugging")
	in the source code before the #include <assert.h> directive,
	the effect is to comment out the assert statement.
	*/
	assert(next != IS);

	/*
	The other way to include diagnostics in a program is to use
	conditional preprocessing as shown bellow. It allows the programmer
	to send more details describing the run-time problem.
	Once the program is tested thoroughly #define DEBUG is commented out
	or #undef DEBUF is used - see the top of the file.
	*/
#ifdef DEBUG
	if (next == IS) {
		printf("Scanner Error: Illegal state:\n");
		printf("Input symbol: %c Row: %d Column: %d\n", c, state, col);
		exit(1);
	}
#endif
	return next;
}

/*Function name: nextTokenClass
Purpose: Due to not using above function this also goes un-used.
Author: Paulo Sousa
History/Versions: 1.0, 15 Nov 2020
Called functions: nextTokenClass(c)
Return value: currentToken;
*/
int nextTokenClass(char c) {
	int val = -1;
	if (debugMode)
		fprintf(stderr, "[04] NextTokenClass\n");

	/*
	THIS FUNCTION RETURNS THE COLUMN NUMBER IN THE TRANSITION
	TABLE st_table FOR THE INPUT CHARACTER c.
	SOME COLUMNS MAY REPRESENT A CHARACTER CLASS .
	FOR EXAMPLE IF COLUMN 2 REPRESENTS [A-Za-z]
	THE FUNCTION RETURNS 2 EVERY TIME c IS ONE
	OF THE LETTERS A,B,...,Z,a,b...z.
	PAY ATTENTION THAT THE FIRST COLOMN IN THE TT IS 0 (has index 0)
	*/

	/* TODO_05: the logic to return the next column in TT */
	/*				[A-z](0),	0(1),	[1-9](2),	.(3),	$(4),	"(5),	SEOF(6),	other(7) */
	switch (c) {
	case 2:
		break;
	}
	return val;
}

/*************************************************************
 * Acceptance State Function AVID
 ************************************************************/
 /* TODO_06: Implement the method to recognize AVID */
 /*
HERE YOU WRITE THE DEFINITIONS FOR YOUR ACCEPTING FUNCTIONS.
************************************************************
/*Function name: aStateFuncAVID
Purpose: take lexeme string and see if lexeme string passes AVID criteria
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Parameters: char lexeme[]
Return value: int;
Algorithm: loop through string see if only digits and alphabet is present and no invalid chars are in string*/
int aStateFuncAVID(char lexeme[]) {

	int firstCharisAlpa = 0; /*used to see if first char is alpabet*/

	stringLexem[8] = '\0'; /*set avid to a 8 char limit*/

	/*check if first char is alphabet or if space is null*/
	if (isalpha(lexeme[0]) || charPointLexem > 1) {
		firstCharisAlpa = 1;
	}

	/*loop thorugh string to see if any invalid characters are present*/
	for (int i = 0; i < 154; i++) {

		/*if end symbol or whitespace skip*/
		if (lexeme[i] == '\0' && firstCharisAlpa == 1 || lexeme[i] == ' ' && firstCharisAlpa == 1) {

		}
		/*else if is not digit or alpha return 0*/
		else if (!isdigit(lexeme[i]) && !isalpha(lexeme[i])) {
			return 0;
		}

	}

	/*if first char is alpha return 1*/
	if (firstCharisAlpa == 1) {
		return 1;
	}

	/*default return 1*/
	return 0;
}

/*************************************************************
 * Acceptance State Function SVID
 ************************************************************/
 /* TODO_07: Implement the method to recognize SVID */
/*Function name: aStateFuncSVID
Purpose: take lexeme string and see if lexeme string passes SVID criteria
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Parameters: char lexeme[]
Return value: int;
Algorithm: loop through string see if only digits and alphabet is present and no invalid chars are in string*/
int aStateFuncSVID(char lexeme[]) {

	int lastCharisDollar = 0;/*used to see if last char is dollar*/

	/*loop through string to see if \0 is hit*/
	for (int i = 0; i < 154; i++) {
		/*if \0 hit look back one char point if is $ then $ is last char in string*/
		if (lexeme[i] == '$' && i - 1 != 0 && lexeme[i - 1] != '\0') {
			lastCharisDollar = 1;
		}
	}

	/*check if valid char is ethier alphabet or digit*/
	for (int i = 0; i < 154; i++) {
		if (lexeme[i] == '\0' && lastCharisDollar == 1 || lexeme[i] == '$') {

		}
		/*if not digit or alpha the return 0*/
		else if (!isdigit(lexeme[i]) && !isalpha(lexeme[i])) {
			return 0;
		}
	}

	/*if passed for loop and $ is last char return 1*/
	if (lastCharisDollar == 1) {
		return 1;
	}

	/*default return 0*/
	return 0;
}

/*************************************************************
 * Acceptance State Function IL
 ************************************************************/
 /* TODO_08: Implement the method to recognize IL */

/*Function name: aStateFuncIL
Purpose: take lexeme string and see if lexeme string passes IL criteria
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Parameters: char lexeme[]
Return value: int;
Algorithm: loop through string see if only digits is present and no invalid chars are in string*/
int aStateFuncIL(char lexeme[]) {
	Token currentToken = { 0 };
	int intValue = 0; /*see if some chars in string is int*/

	/*loop through string to see if int is possible*/
	for (int i = 0; i < 154; i++) {
		/*if char is int there is a possible int*/
		if (isdigit(lexeme[i])) {
			intValue = 1;
		}
	}
	/*loop through string to see if all chars are int*/
	for (int i = 0; i < 154; i++) {
		if (lexeme[i] == '\0' && intValue == 1) {

		}
		/*if not digit the return 0*/
		else if (!isdigit(lexeme[i])) {
			return 0;
		}
	}

	/*default return 1*/
	return 1;
}
/*************************************************************
 * Acceptance State Function FPL
 ************************************************************/
 /* TODO_09: Implement the method to recognize FPL */

/*Function name: aStateFuncFPL
Purpose: take lexeme string and see if lexeme string passes FPL criteria
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Parameters: char lexeme[]
Return value: int;
Algorithm: loop through string see if only digits and one decimal is present and no invalid chars are in string*/
int aStateFuncFPL(char lexeme[]) {

	int intValue = 0;	/*used to see if int values in string*/
	int pointValue = 0;	/*used to see if a decimal is in string*/

	/*loop through string to see if a int is in string and if a decimal is in string*/
	for (int i = 0; i < 154; i++) {
		if (isdigit(lexeme[i])) {
			intValue = 1;
		}
		if (lexeme[i] == '.') {
			pointValue = 1;
		}
	}
	/*loop through string to see any chars others then digit or decimal are in string*/
	for (int i = 0; i < 154; i++) {
		if (lexeme[i] == '\0' || lexeme[i] == '\n' && intValue == 1) {

		}
		/*if not digit char and not decimal return 0*/
		else if (!isdigit(lexeme[i]) && lexeme[i] != '.') {
			return 0;
		}
	}

	/*if no decimal in string return 0*/
	if (pointValue != 1) {
		return 0;
	}

	/*default return 1*/
	return 1;
}

/*************************************************************
 * Acceptance State Function SL
 ************************************************************/
 /* TODO_10: Implement the method to recognize SL */

/*Function name: aStateFuncSL
Purpose: get size of valid SL(string) passed
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Parameters: char lexeme[]
Algorithm: loop through string and count how many characters are in use*/
void aStateFuncSL(char lexeme[]) {

	/*loop through string count howmany chars used*/
	for (int i = 0; i < 150; i++) {
		/*if char is not end of string keep adding to size*/
		if (lexeme[i] != '\0') {
			sizeofString++;
		}


	}

}

/*************************************************************
 * Acceptance State Function Error
 ************************************************************/
 /* TODO_11: Implement the method to deal with Error Token */
 /*
 ACCEPTING FUNCTION FOR THE ERROR TOKEN
 */

 /*Function name: aStateFuncErr
 Purpose: spot potential error in lexeme string before checking for end state
 Author: Asim Jasarevic
 History/Versions: 1.0, 15 Nov 2020
 Parameters: char lexeme[]
 Return value: int;
 Algorithm: go through string and put it through a bunch of test cases to see if an err flag can be raised */
int aStateFuncErr(char lexeme[]) {

	/*check if a invalid comment is present if yes return 5*/
	if (strstr(lexeme, "!!") == NULL && strstr(lexeme, "!") != NULL) {
		return 5;
	}

	int numOfOthers = 0; /*number of other chars not alpha and not digit*/
	int numOfAlpha = 0;	 /*number of alphabet chars in string*/
	int numOfDigits = 0; /*number of digit chars in string*/
	int numberCheck = 1;  /*used to see if only numbers are in string*/
	int pointCheck = 0;  /*used to see if a decimal is in string*/
	int quoteCheck = 0;	 /*used to see if a " is in string*/

	/*loop string to see if decimal in string*/
	if (strstr(lexeme, ".")) {
		pointCheck = 1;
	}

	/*check if first char is . if so error return 2*/
	if (lexeme[0] == '.') {
		return 2;
	}

	/*check if first char is = and no chars come after is no error just clear lexeme return 0*/
	if (lexeme[0] == '=' && lexeme[1] == '\0') {
		return 0;
	}

	/*check if second char is " if so quoteCheck = 0*/
	if (lexeme[1] == '"') {
		quoteCheck = 0;
	}

	/*loop through string to set numOf vars*/
	for (int i = 0; i < 150; i++) {
		/*if char is not digit or decimal see if alpha or other character*/
		if (!isdigit(lexeme[i]) && lexeme[i] != '.' && lexeme[i] != '\0') {
			/*if char is not alpha or int increment numOfOther*/
			if (!isalpha(lexeme[i]) && !isalpha(lexeme[i])) {
				numOfOthers++;
			}
			/*if char is alpha increment numOfAlpha*/
			if (isalpha(lexeme[i])) {
				numOfAlpha++;
			}
			numberCheck = 0;
		}
		/*if char is digit increment numOfAlpha*/
		if (isdigit(lexeme[i])) {
			numOfDigits++;
		}
	}

	/*if numberCheck == 0 && pointCheck == 1 there is a flt error with a fix*/
	if (numberCheck == 0 && pointCheck == 1) {
		/*loop through string see where invalid char lies and move chars in string*/
		for (int i = 0; i < 150; i++) {
			/*if invalid char set charErr to char stringLexem at i to end of string to cut off any invalid chars and set charErrCalled to 1 to notify processToken()*/
			if (!isdigit(lexeme[i]) && lexeme[i] != '.' && lexeme[i] != '\0') {
				charErr = lexeme[i];
				stringLexem[i] = '\0';
				charErrCalled = 1;
				return 6;
			}
		}
	}

	/* ifnumberCheck == 0 && pointCheck == 0 && numOfOthers == 1 && numOfDigits == 1 there is an int error with a fix*/
	else if (numberCheck == 0 && pointCheck == 0 && numOfOthers == 1 && numOfDigits == 1) {
		/*loop through string see where invalid char lies and move chars in string*/
		for (int i = 0; i < 150; i++) {
			/*if invalid char set charErr to char stringLexem at i to end of string to cut off any invalid chars and set charErrCalled to 1 to notify processToken()*/
			if (!isdigit(lexeme[i]) && lexeme[i] != '.' && lexeme[i] != '\0') {
				charErr = lexeme[i];
				stringLexem[i] = '\0';
				charErrCalled = 1;
				return 6;
			}
		}
	}

	/* if numberCheck == 1 && pointCheck == 1 there may be an flt err so chek if size is good*/
	else if (numberCheck == 1 && pointCheck == 1) {
		/*if string to flt bigger then SHRT_MAX error return 1*/
		if (atof(lexeme) > SHRT_MAX) {
			return 1;
		}

		/*for all chars after 20 see if a number is present if so move it up to 20th place in string to not overload token */
		for (int i = 19; i < 150; i++) {
			if (lexeme[i] != '0' && lexeme[i] != '.' && lexeme[i] != '\0') {
				lexeme[18] = lexeme[i];
				stringLexem[18] = lexeme[i];
			}
			stringLexem[i] = '\0';
			lexeme[i] = '\0';
		}

		/*if string to flt smaleer then 0.0000000000000001 error return 1*/
		if (atof(lexeme) < 0.0000000000000001 && atof(lexeme) != 0) {
			return 1;
		}
	}

	/* if numberCheck == 1 && pointCheck == 0 there may be an int err so chek if size is good*/
	else if (numberCheck == 1 && pointCheck == 0) {
		/*if string to int is bigger or smaller then SHRT_MAX/SHRT_MIN error return 2*/
		if (atoi(lexeme) > SHRT_MAX || atoi(lexeme) < SHRT_MIN) {
			return 2;
		}
	}

	/* if numberCheck == 0 && quoteCheck == 1 there may be an str err so check if two " are present*/
	else if (numberCheck == 0 && quoteCheck == 1) {

		/*if first har in string is " start*/
		if (lexeme[0] == '"') {
			/*loop through string if second " found but next char is not end of string error return 4*/
			for (int i = 1; i < 150; i++) {
				if (lexeme[i] == '"' && lexeme[i + 1] != '/0') {
					return 4;
				}
			}
		}

	}

	/* if numberCheck == 0 and all other elses are passed check if _AND/_NOT/_OR are not in string if they are not an error but clear lexeme string to prevent future reading
		if not a _AND/_NOT/_OR issue then a AVID error may exsist */
	else if (numberCheck == 0) {

		if (strstr(lexeme, "_AND") || strstr(lexeme, "_NOT") || strstr(lexeme, "_OR")) {
			return 0;
		}

		/*loop through string see if other char then alpha digit whitespace or $ exsist*/
		for (int i = 0; i < 150; i++) {

			/*if other char found set charErr to char add end of string to stringLexem to prevent more error characters in front return 3*/
			if (!isdigit(lexeme[i]) && !isalpha(lexeme[i]) && lexeme[i] != '\0' && lexeme[i] != '\n' && lexeme[i] != ' ' && lexeme[i] != '$') {
				charErr = lexeme[i];
				charErrCalled = 1;
				stringLexem[i] = ' ';
				return 3;
			}

		}

	}

	/*if nothing hit no error*/
	return 0;
}

/*************************************************************
 * Check if it is Keyword
 ************************************************************/
 /* TODO_11: Checks if a string is a keyword, returning the position in the list */

/*Function name: isKeyword
Purpose: find keywords in lexeme string
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Parameters: char lexeme[]
Return value: int;
Algorithm: loop though keywordTable to see if a keyword exsists in lexeme string*/
int isKeyword(char* kw_lexeme) {
	/*loop through all points in keywordTable*/
	for (int i = 0; i < 10; i++) {
		/* if a keyword is in a lexeme string return int of index where if was hit*/
		if ((strstr(kw_lexeme, keywordTable[i]) != NULL)) {
			return i;
		}
	}
	return -1;
}

/*
HERE YOU WRITE YOUR ADDITIONAL FUNCTIONS (IF ANY).
FOR EXAMPLE
*/

/*Function name: checkIfLexemHeld
Purpose: check if next or current char will flag a state
Author: Asim Jasarevic
History/Versions: 1.0, 15 Nov 2020
Parameters: char char* string, char* subString, char c2, char c
Return value: int;
Algorithm: heck if next or current char will flag a state if so set startLexemCheck to let processToken know*/
void checkIfLexemHeld(char* string, char* subString, char c2, char c) {
	/*if next char is a whitespace raise flag*/
	if (c2 == ' ' && !strstr(string, "!!") || c2 == '\0' && !strstr(string, "!!") || c2 == '\n' && !strstr(string, "!!")) {
		startLexemCheck = 1;
	}
	/*if next char is a { raise flag*/
	else if (c2 == '{' && !strstr(string, "!!")) {
		startLexemCheck = 1;
	}
	/*if next char is a } raise flag*/
	else if (c2 == '}' && !strstr(string, "!!")) {
		startLexemCheck = 1;
	}
	/*if next char is a , raise flag*/
	else if (c2 == ',' && !strstr(string, "!!")) {
		startLexemCheck = 1;
	}
	/*if next char is a = raise flag*/
	else if (c2 == '=' && !strstr(string, "!!")) {
		startLexemCheck = 1;
	}
	/*if current char is a = raise flag*/
	else if (c == '=' && c2 == '=' && !strstr(string, "!!")) {
		startLexemCheck = 1;
	}
	/*if current char is a $ raise flag*/
	else if (c == '$' && !strstr(string, "!!")) {
		startLexemCheck = 1;
	}
	/*if next char is a ( raise flag*/
	else if (c2 == '(' && !strstr(string, "!!")) {
		startLexemCheck = 1;
	}
	/*if next char is a > raise flag*/
	else if (c2 == '>' && !strstr(string, "!!")) {
		startLexemCheck = 1;
	}
	else if (c2 == '_' && !strstr(string, "!!")) {
		startLexemCheck = 1;
	}
	/*if next char is a < raise flag*/
	else if (c2 == '<' && !strstr(string, "!!")) {
		startLexemCheck = 1;
	}
	/*if next char is a ) raise flag*/
	else if (c2 == ')' && !strstr(string, "!!")) {
		startLexemCheck = 1;
	}
	/*if next char is a + or - or * or / raise flag*/
	else if (c2 == '+' && !strstr(string, "!!") || c2 == '-' && !strstr(string, "!!") || c2 == '*' && !strstr(string, "!!") || c2 == '/' && !strstr(string, "!!")) {
		startLexemCheck = 1;
	}
	/*if next char is a ; raise flag*/
	else if (c2 == ';' && !strstr(string, "!!")) {
		startLexemCheck = 1;
	}
}
