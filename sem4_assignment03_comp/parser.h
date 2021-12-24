/*
* File Name: parser.h
* Compiler: MS Visual Studio 2019
* Author: Asim Jasarevic
* Course: CST 8152 – Compilers, Lab Section: 013
* Assignment: 3
* Date: 13 Dec 2020
* Professor: Paulo Sousa / Haider Miraj
* Purpose: define/import global vars, constants, and functions to be used in parser.c
*/

/* Inclusion section */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "token.h"
#include "buffer.h"


/* Global vars */
int syntaxErrorNumber; /*number of syntax error in a file*/
extern Buffer* stringLiteralTable; /*struct to hold string literals found by scanner*/
extern int line; /*line number read*/
extern Token processToken(); /*struct token used to form grammer */
extern char* keywordTable[]; /*keyword tabel used to hold all keywords used in PLAYTAPUS*/
static Token lookahead; /*struct to get next token*/

/* Key words used in PLAYTAPUS*/
#define	NO_ATTR (-1)
#define INPUT	(0)
#define PROGRAM (1)
#define DO (2)
#define WHILE (3)
#define TRUE (4)
#define IF (5)
#define THEN (6)
#define ELSE (7)
#define OUTPUT (8)
#define FALSE (9)

/* Function definitions */
void processParser(void); /*starts parsing process*/
void matchToken(int, int); /*checks if current token matches ecpected token in grammer*/
void syncErrorHandler(int); /*if no match function makes sure to start new grammer*/
void printError(); /*if no match function decides what error to print*/
void getNextTKN(); /*skip over any NULL tokens found*/

// TODO_04 All FunctionsForNonTerminals
void program();
void optionalStatements();
void statements();
void statement();
void statementsPrime();
void assignmentStatement();
void assignmentExpression();
void selectionStatement();
void iterationStatement();
void preCondition();
void inputStatement();
void variableList();
void variableIdentifier();
void variableListPrime();
void outputSatement();
void output_list();
void optVariableList();
void arithmeticExpression();
void unaryArithmeticExpression();
void additiveArithmeticExpression();
void additiveArithmeticExpressionPrime();
void multiplicativeArithmeticExpression();
void multiplicativeArithmeticExpressionPrime();
void primaryArithmeticExpression();
void stringExpression();
void primaryStringExpression();
void stringExpressionPrime();
void conditionalExpression();
void logicalOrExpression();
void logicalOrExpressionPrime();
void logicalAndExpression();
void logicalAndExpressionPrime();
void relationalExpression();
void operator_list();
void primary_a_relationalExpression();
void primary_s_relationalExpression(); 
