/*
* File Name: token.h
* Compiler: MS Visual Studio 2019
* Author: Asim Jasarevic
* Course: CST 8152 – Compilers, Lab Section: 013
* Assignment: 2
* Date: 15 Nov 2020
* Professor: Paulo Sousa / Haider Miraj
* Purpose: define constant and states along with seting up values for struct to hold and be called
*/

#ifndef TOKEN_H_
#define TOKEN_H_

/*#pragma warning(1:4001) *//*to enforce C89 type comments  - to make //comments an warning */

/*#pragma warning(error:4001)*//* to enforce C89 comments - to make // comments an error */

/* Constants */
#define VID_LEN 8   /* variable identifier length */
#define ERR_LEN 20  /* error message length */
#define INL_LEN 5   /* maximum number of digits for IL */

/* Token codes: Numbers from -1 to 18 (old: 0 to 19) */
#define ERR_T     -1 /* Error token */
#define SEOF_T    0  /* Source end-of-file token (DONE)*/
#define AVID_T    1  /* Arithmetic Variable identifier token (DONE)*/
#define SVID_T    2  /* String Variable identifier token: Ending in $ (old: @) (DONE)*/
#define FPL_T     3  /* Floating point literal token (DONE)*/
#define INL_T     4  /* Integer literal token (DONE)*/
#define STR_T     5  /* String literal token */
#define SCC_OP_T  6  /* String concatenation operator token: $$ (old: ## or >>) (DONE)*/
#define ASS_OP_T  7  /* Assignment operator token (DONE)*/
#define ART_OP_T  8  /* Arithmetic operator token (DONE)*/
#define REL_OP_T  9  /* Relational operator token (DONE)*/ 
#define LOG_OP_T 10  /* Logical operator token (DONE)*/
#define LPR_T    11  /* Left parenthesis token (DONE)*/
#define RPR_T    12  /* Right parenthesis token (DONE)*/
#define LBR_T    13  /* Left brace token (DONE)*/
#define RBR_T    14  /* Right brace token (DONE)*/
#define KW_T     15  /* Keyword token (DONE)*/
#define COM_T    16  /* Comma token (DONE)*/
#define EOS_T    17  /* End of statement (semicolon) (DONE)*/
#define RTE_T    18  /* Run-time error token */
#define NLL_T	 19	 /* null token */

/* Operators token attributes */
typedef enum ArithmeticOperators { ADD, SUB, MUL, DIV } Arr_Op;
typedef enum RelationalOperators { EQ, NE, GT, LT } Rel_Op;
typedef enum LogicalOperators { AND, OR, NOT } Log_Op;
typedef enum SourceEndOfFile { SEOF_0, SEOF_EOF } S_Eof;

/* Data structures for declaring the token and its attributes */
typedef union TokenAttribute {
	int get_int;      /* integer attributes accessor */
	Arr_Op arr_op;    /* arithmetic operator attribute code */
	Rel_Op rel_op;    /* relational operator attribute code */
	Log_Op log_op;    /* logical operator attribute code */
	S_Eof seof;        /* source-end-of-file attribute code */
	int int_value;    /* integer literal attribute (value) */
	int kwt_idx;      /* keyword index in the keyword table */
	short str_offset; /* sring literal offset from the beginning of the string literal buffer (str_LTBL->cb_head) */
	float flt_value;    /* floating-point literal attribute (value) */
	char vid_lex[VID_LEN + 1]; /* variable identifier token attribute */
	char err_lex[ERR_LEN + 1]; /* error token attribite */
} TokenAtt;

/* Should be used if no symbol table is implemented */
typedef struct vidTokenAttibutes {
	unsigned char flags;
	union {
		short int_value;
		float flt_value;
		void* str_locator;
	} values;
} VidAtt;

/* Token declaration */
typedef struct Token {
	int code;     /* token code */
	TokenAtt attribute; /* token attribute */
	VidAtt avid_attribute; /* not used in this scanner implementation - for further use */
} Token;

#endif
