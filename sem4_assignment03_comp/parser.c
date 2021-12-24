/*
* File Name: parser.c
* Compiler: MS Visual Studio 2019
* Author: Asim Jasarevic
* Course: CST 8152 – Compilers, Lab Section: 013
* Assignment: 3
* Date: 13 Dec 2020
* Professor: Paulo Sousa / Haider Miraj
* Purpose: Use tokens to try and match grammer, if a match is made parse complete grammer, if not detect syntax error and print error
*/

#include "parser.h"


/*Function name: processParser
Purpose: start of parser calls first token and makes sure program starts with PROGRAM{ 
Author: Asim Jasarevic
History/Versions: 1.0, 13 Dec 2020
Called functions: processToken(), getNextTKN(), program(), matchToken()
Algorithm: get token call program() to see if token match "PROGRAM{}" end when SEOF token is called */
void processParser() {

	lookahead = processToken();
	getNextTKN();
	program();
	matchToken(SEOF_T, NO_ATTR);
	printf("%s\n", "PLATY: Source file parsed");

}

/*Function name: matchToken
Purpose: see if passed params match current token
Author: Asim Jasarevic
History/Versions: 1.0, 13 Dec 2020
Called functions: syncErrorHandler(), processToken(), getNextTKN(), printError()
Algorithm: see if passed params match current token if not parser error ocured, is error start new grammer with new token*/
void matchToken(int tokenCode, int tokenAttribute) {

	/*if current token does not match passed values call syncErrorHandler()*/
	if (lookahead.code != tokenCode) {
		syncErrorHandler(tokenCode);
		return;
	}

	/*if token has any of the following codes call syncErrorHandler()*/
	switch (tokenCode) {
	case KW_T:
	case REL_OP_T:
	case ART_OP_T:
	case LOG_OP_T:
		if (lookahead.attribute.get_int != tokenAttribute) {
			syncErrorHandler(tokenCode);
			return;
		}

	}

	/*if token is end of file exit parser body*/
	if (lookahead.code == SEOF_T) {
		return;
	}

	/*get next token*/
	lookahead = processToken();
	getNextTKN();

	/*if the current token is has an error code process error and get next token*/
	if (lookahead.code == ERR_T) {
		printError();
		syntaxErrorNumber++;
		lookahead = processToken();
		getNextTKN();
	}
}

/*Function name: syncErrorHandler
Purpose: get next token to start new grammer 
Author: Asim Jasarevic
History/Versions: 1.0, 13 Dec 2020
Called functions: exit(), processToken(), getNextTKN(), printError()
Algorithm: while the current token does not equal the passed code get new token and if end of file exit parser body*/
void syncErrorHandler(int syncTokenCode) {

	printError();
	syntaxErrorNumber++;

	/*get next token until match*/
	while (lookahead.code != syncTokenCode) {
		/*see if current token end of file*/
		if (lookahead.code == SEOF_T) {
			exit(0);
		}

		/*Get the next token*/
		lookahead = processToken();
		getNextTKN();
	}

	/*after token match is found make sure it's not end of file*/
	if (syncTokenCode != SEOF_T) {
		lookahead = processToken();
		getNextTKN();
	}

	return;
}


/*Function name: printError
Purpose: print error depending on TKN code
Author: Asim Jasarevic
History/Versions: 1.0, 13 Dec 2020
Called functions: exit(), processToken(), getNextTKN(), printError()
Algorithm: switch case to find current tokn code, once found print error specific text and TKN code */
void printError() {

	Token t = lookahead;
	syntaxErrorNumber++;

	printf("PLATY: Syntax error:  Line:%3d\n", line);
	printf("*****  Token code:%3d Attribute: ", t.code);
	
	/*find current TKN code and output specific text*/
	switch (t.code) {

	case INL_T: 
		printf("%d\n", t.attribute.get_int);
		break;

	case  ART_OP_T:
		printf("%d\n", t.attribute.get_int);
		break;

	case  REL_OP_T:
		printf("%d\n", t.attribute.get_int);
		break;

	case  LOG_OP_T:
		printf("%d\n", t.attribute.get_int);
		break;

	case  AVID_T:
	case  SVID_T:
		printf("%s\n", t.attribute.vid_lex);
		break;

	case KW_T:
		printf("%s\n", keywordTable[t.attribute.get_int]);
		break;

	case  ERR_T:
		printf("%s\n", t.attribute.err_lex);
		break;

	case  SEOF_T:
		printf("SEOF_T\t\t%d\t\n", t.attribute.seof);
		break;

	case  FPL_T:
		printf("%lf\n", t.attribute.flt_value);
		break;

	case COM_T:
		printf("NA\n");
		break;

	case EOS_T:
		printf("NA\n");
		break;

	case SCC_OP_T: 
		printf("NA\n");
		break;

	case  LPR_T: 
		printf("NA\n");
		break;

	case  RPR_T: 
		printf("NA\n");
		break;

	case LBR_T: 
		printf("NA\n");
		break;

	case RBR_T: 
		printf("NA\n");
		break;

	case  ASS_OP_T:
		printf("NA\n");
		break;

	default:
		printf("PLATY: Scanner error: invalid token code: %d\n", t.code);
	}
	
}



/*Function name: program
Purpose: parse through program 
Author: Asim Jasarevic
History/Versions: 1.0, 13 Dec 2020
Called functions: matchToken(), optionalStatements()
Algorithm: match PROGRAM, {, and } also parse evrything between {}*/
 void program() {

	matchToken(KW_T, PROGRAM);
	matchToken(LBR_T, NO_ATTR);
	optionalStatements();

	matchToken(RBR_T, NO_ATTR);
	printf("%s\n", "PLATY: Program parsed");

}

 /*Function name: optionalStatements
 Purpose: parse through optionalStatements
 Author: Asim Jasarevic
 History/Versions: 1.0, 13 Dec 2020
 Called functions: statements()
 Algorithm: match AVID/SVID and KW_T also parse evrything between AVID/SVID and KW_T
			then match IF/WHILE/INPUT/OUTPUT and parse evrything after until ; hit*/
 void optionalStatements() {

	 switch (lookahead.code) {
	 case AVID_T: 
	 case SVID_T:
		 statements();
		 break;

	 case KW_T:
		 /*check for IF, WHILE, INPUT, OUTPUT token code and check what comes after*/
		 if (lookahead.attribute.get_int == IF || lookahead.attribute.get_int == WHILE || lookahead.attribute.get_int == INPUT || lookahead.attribute.get_int == OUTPUT) {
			 statements();
			 break;
		 }

	 default: 
		 printf("PLATY: Opt_statements parsed\n");
	 }

 }

/*Function name: statements
Purpose: parse through statements
Author: Asim Jasarevic
History/Versions: 1.0, 13 Dec 2020
Called functions: statement(), statementsPrime()
Algorithm: call statement and statementsPrime to connect grammer*/
 void statements() {

	 statement();
	 statementsPrime();

 }

 /*Function name: statement
 Purpose: parse through statement
 Author: Asim Jasarevic
 History/Versions: 1.0, 13 Dec 2020
 Called functions: assignmentStatement(), selectionStatement(), iterationStatement(),  outputSatement(), inputStatement(), printError()
 Algorithm: check if token is KW_T or AVID?SVID and pass a statment depending on KW_t else error occured*/
 void statement() {

	 switch (lookahead.code) {
	 case AVID_T: case SVID_T:
		 assignmentStatement();
		 break;

	 case KW_T:
		 /*check for IF, WHILE, READ, OUTPUT*/
		 if (lookahead.attribute.get_int == IF) {
			 selectionStatement();
		 }
		 else if (lookahead.attribute.get_int == WHILE) {
			 iterationStatement();
		 }
		 else if (lookahead.attribute.get_int == OUTPUT) {
			 outputSatement();
		 }
		 else if (lookahead.attribute.get_int == INPUT) {
			 inputStatement();
		 }
		 break;

	 default:
		 /*Else it is an error*/
		 printError();
		 break;
	 }

 }


 /*Function name: statementsPrime
  Purpose: parse through statementsPrime
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: statement(), statementsPrime()
  Algorithm: check if token is KW_T or AVID/SVID and pass a statment depending on KW_t*/
 void statementsPrime() {

	 switch (lookahead.code) {
	 case KW_T:
		 /*check for IF, WHILE, READ, OUTPUT*/
		 if (lookahead.attribute.get_int == INPUT || lookahead.attribute.get_int == OUTPUT || lookahead.attribute.get_int == IF || lookahead.attribute.get_int == WHILE) {
			 statement();
			 statementsPrime();
		 }
		 break;

	 case AVID_T: case SVID_T:
		 statement();
		 statementsPrime();
		 break;

	 }

 }

 /*Function name: assignmentStatement
  Purpose: parse through assignmentStatement
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: assignmentExpression(), matchToken()
  Algorithm: check for next assignmentExpression and after found find ";"*/
 void assignmentStatement() {

	 assignmentExpression();

	 /*get ";" to end assignmentStatement*/
	 matchToken(EOS_T, NO_ATTR);

	 printf("PLATY: Assignment statement parsed\n");

 }

 /*Function name: assignmentExpression
  Purpose: parse through assignmentExpression
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: matchToken(), arithmeticExpression()
  Algorithm: check for AVID/SVID then ASS_OP_T then move to arithmeticExpression once ran output Assignment expression*/
 void assignmentExpression() {

	 switch (lookahead.code) {
	 case AVID_T:
		 matchToken(AVID_T, NO_ATTR);
		 matchToken(ASS_OP_T, NO_ATTR);
		 arithmeticExpression();

		 printf("PLATY: Assignment expression (arithmetic) parsed\n");
		 break;

	 case SVID_T:
		 matchToken(SVID_T, NO_ATTR);
		 matchToken(ASS_OP_T, NO_ATTR);
		 stringExpression();

		 printf("PLATY: Assignment expression (string) parsed\n");
		 break;

	 default:
		 printError();
		 break;
	 }

 }

 /*Function name: selectionStatement
  Purpose: parse through selectionStatement
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: preCondition(), conditionalExpression(), optionalStatements()
  Algorithm: check for "IF" then run preCondition, check for "(THEN)" run optionalStatements, check for "(ELSE)" run optionalStatements find ";" to end grammar*/
 void selectionStatement() {

	 matchToken(KW_T, IF);
	 preCondition();

	 matchToken(LPR_T, NO_ATTR);
	 conditionalExpression();

	 matchToken(RPR_T, NO_ATTR);
	 matchToken(KW_T, THEN);
	 matchToken(LBR_T, NO_ATTR);
	 optionalStatements();

	 matchToken(RBR_T, NO_ATTR);
	 matchToken(KW_T, ELSE);
	 matchToken(LBR_T, NO_ATTR);
	 optionalStatements();

	 matchToken(RBR_T, NO_ATTR);
	 matchToken(EOS_T, NO_ATTR);

	 printf("PLATY: Selection statement parsed\n");

 }

 /*Function name: iterationStatement
  Purpose: parse through iterationStatement
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: preCondition(), conditionalExpression(), statements()
  Algorithm: check for "WHILE" then run preCondition, check for "(<conditionalExpression>)" , check for "(DO)" run statements find ";" to end grammar*/
 void iterationStatement() {

	 matchToken(KW_T, WHILE);
	 preCondition();

	 matchToken(LPR_T, NO_ATTR);
	 conditionalExpression();

	 matchToken(RPR_T, NO_ATTR);
	 matchToken(KW_T, DO);
	 matchToken(LBR_T, NO_ATTR);
	 statements();

	 matchToken(RBR_T, NO_ATTR);
	 matchToken(EOS_T, NO_ATTR);

	 printf("PLATY: Iteration statement parsed\n");

 }

 /*Function name: preCondition
  Purpose: parse through preCondition
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: matchToken(), printError()
  Algorithm: check for TRUE or FALSE if found run match token else call error*/
 void preCondition() {

	 switch (lookahead.code) {
	 case KW_T:

		 /*if TKN = TRUE/FALSE match TKN else call error*/
		 if (lookahead.attribute.get_int == TRUE) {
			 matchToken(KW_T, TRUE);
		 }
		 else if (lookahead.attribute.get_int == FALSE) {
			 matchToken(KW_T, FALSE);
		 }
		 else {
			 printError();
		 }
		 break;

	 default:
		 printError();
		 break;
	 }

 }

 /*Function name: inputStatement
  Purpose: parse through inputStatement
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: matchToken(), variableList()
  Algorithm: match for "INPUT(<variableList>);" and print statement*/
 void inputStatement() {

	 matchToken(KW_T, INPUT);
	 matchToken(LPR_T, NO_ATTR);
	 variableList();
	 matchToken(RPR_T, NO_ATTR);
	 matchToken(EOS_T, NO_ATTR);

	 printf("PLATY: Input statement parsed\n");

 }

 /*Function name: variableList
  Purpose: parse through variableList
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: variableIdentifier(), variableListPrime()
  Algorithm: call two function to find AVID's/SIVD's for variable list*/
 void variableList() {

	 variableIdentifier();
	 variableListPrime();

	 printf("PLATY: Variable list parsed\n");

 }

 /*Function name: variableIdentifier
  Purpose: parse through variableIdentifier
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: matchToken()
  Algorithm: find AVID's/SIVD's for variable list if none found call error*/
 void variableIdentifier() {

	 switch (lookahead.code) {
	 case AVID_T:
		 matchToken(AVID_T, NO_ATTR);
		 break;
	 case SVID_T:
		 matchToken(SVID_T, NO_ATTR);
		 break;
	 default:
		 printError();
		 break;
	 }

 }

 /*Function name: variableListPrime
  Purpose: parse through variableListPrime
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: matchToken()
  Algorithm: find AVID's/SIVD's for variable list if none found call error*/
 void variableListPrime() {

	 switch (lookahead.code) {
	 case COM_T:
		 matchToken(COM_T, NO_ATTR);
		 variableIdentifier();
		 variableListPrime();
		 break;
	 }

 }

 /*Function name: outputSatement
  Purpose: parse through outputSatement
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: matchToken(), output_list()
  Algorithm: match "OUTPUT(<output_list>);" and print a statement*/
 void outputSatement() {

	 matchToken(KW_T, OUTPUT);
	 matchToken(LPR_T, NO_ATTR);
	 output_list();
	 matchToken(RPR_T, NO_ATTR);
	 matchToken(EOS_T, NO_ATTR);

	 printf("PLATY: Output statement parsed\n");

 }

 /*Function name: output_list
  Purpose: parse through output_list
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: matchToken(), optVariableList()
  Algorithm: try to find STR_T TKN if found parse grammar and start new else move foward and pass to optVariableList*/
 void output_list() {

	 switch (lookahead.code) {
	 case STR_T:
		 matchToken(STR_T, NO_ATTR);
		 printf("PLATY: Output list (string literal) parsed\n");
		 break;

	 default:
		 optVariableList();
		 break;
	 }

 }

 /*Function name: optVariableList
  Purpose: parse through optVariableList
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: variableList()
  Algorithm: try to find AVID_T/SVID_T TKN if found parse variableList and start new grammar else print empty output and start new grammar*/
 void optVariableList() {

	 switch (lookahead.code) {
	 case AVID_T: case SVID_T:
		 variableList();
		 break;

	 default:
		 printf("PLATY: Output list (empty) parsed\n");
		 break;
	 }

 }

 /*Function name: arithmeticExpression
  Purpose: parse through arithmeticExpression
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: unaryArithmeticExpression(), printError(), additiveArithmeticExpression()
  Algorithm: try to find ART_OP_T TKN if found see if +/- parse unaryArithmeticExpression else print error start new grammar.
			 try to find AVID_T/FPL_T/INL_T/LPR_T TKN if found continue grammar to additiveArithmeticExpression else print error start new grammar*/
 void arithmeticExpression() {

	 switch (lookahead.code) {
	 case ART_OP_T:
		 /*if get_int = 0/1 continue grammar*/
		 if (lookahead.attribute.get_int == 0 || lookahead.attribute.get_int == 1) {
			 unaryArithmeticExpression();
		 }
		 else {
			 printError();
			 break;
		 }

		 printf("PLATY: Arithmetic expression parsed\n");
		 break;

	 case AVID_T: case FPL_T: case INL_T: case LPR_T:
		 additiveArithmeticExpression();

		 printf("PLATY: Arithmetic expression parsed\n");
		 break;

	 default:
		 printError();
		 break;
	 }

 }

 /*Function name: unaryArithmeticExpression
  Purpose: parse through unaryArithmeticExpression
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: primaryArithmeticExpression(), printError()
  Algorithm: try to find ART_OP_T TKN if found see if +/- parse primaryArithmeticExpression else print error start new grammar.*/
 void unaryArithmeticExpression() {

	 switch (lookahead.code) {
	 case ART_OP_T:
		 if (lookahead.attribute.get_int == 0 || lookahead.attribute.get_int == 1) {
			 matchToken(ART_OP_T, lookahead.attribute.arr_op);
			 primaryArithmeticExpression();
		 }
		 else {
			 printError();
		 }
		 break;
	 }

	 printf("PLATY: Unary arithmetic expression parsed\n");
 }

 /*Function name: additiveArithmeticExpression
  Purpose: parse through additiveArithmeticExpression
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: multiplicativeArithmeticExpression(), additiveArithmeticExpressionPrime()
  Algorithm: continue grammar to check if multiplicative or additive expresion can be formed.*/
 void additiveArithmeticExpression() {

	 multiplicativeArithmeticExpression();
	 additiveArithmeticExpressionPrime();

 }

 /*Function name: additiveArithmeticExpressionPrime
  Purpose: parse through additiveArithmeticExpressionPrime
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: multiplicativeArithmeticExpression(), additiveArithmeticExpressionPrime()
  Algorithm: check if TKN is ART_OP_T if so check if +/- if so call ArithmeticExpression functions to continue grammar*/
 void additiveArithmeticExpressionPrime() {

	 /*if TKN = ART_OP_T check get_int*/
	 if (lookahead.code == ART_OP_T) {

		 /*if get_int = 0/1 match TKN continue grammar*/
		 if (lookahead.attribute.get_int == 0 || lookahead.attribute.get_int == 1) {

			 matchToken(ART_OP_T, lookahead.attribute.arr_op);
			 multiplicativeArithmeticExpression();
			 additiveArithmeticExpressionPrime();

			 printf("PLATY: Additive arithmetic expression parsed\n");
		 }

	 }

 }

 /*Function name: multiplicativeArithmeticExpression
  Purpose: parse through multiplicativeArithmeticExpression
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: primaryArithmeticExpression(), multiplicativeArithmeticExpressionPrime()
  Algorithm: continue ArithmeticExpression by calling two functions to try and match grammar*/
 void multiplicativeArithmeticExpression() {

	 primaryArithmeticExpression();
	 multiplicativeArithmeticExpressionPrime();

 }

 /*Function name: multiplicativeArithmeticExpressionPrime
  Purpose: parse through multiplicativeArithmeticExpressionPrime
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: matchToken(), primaryArithmeticExpression(), multiplicativeArithmeticExpressionPrime
  Algorithm: if TKN = ART_OP_T check if * or / if so call ArithmeticExpression functions to continue grammar*/
 void multiplicativeArithmeticExpressionPrime() {

	 /*if TKN = ART_OP_T check get_int*/
	 if (lookahead.code == ART_OP_T) {

		 /*if get_int = 2/3 match TKN continue grammar*/
		 if (lookahead.attribute.get_int == 2 || lookahead.attribute.get_int == 3) {

			 matchToken(ART_OP_T, lookahead.attribute.arr_op);
			 primaryArithmeticExpression();
			 multiplicativeArithmeticExpressionPrime();

			 printf("PLATY: Multiplicative arithmetic expression parsed\n");
		 }

	 }

 }

 /*Function name: primaryArithmeticExpression
  Purpose: parse through primaryArithmeticExpression
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: matchToken(), arithmeticExpression(), printError()
  Algorithm: if TKN = AVID_T/FPL_T/INL_T match token and start new grammar if TKN = LPR_T find "(<arithmeticExpression>)" and start new grammar else call error*/
 void primaryArithmeticExpression() {

	 switch (lookahead.code) {
	 case AVID_T: case FPL_T: case INL_T:
		 matchToken(lookahead.code, NO_ATTR);

		 printf("PLATY: Primary arithmetic expression parsed\n");
		 break;

	 case LPR_T:
		 matchToken(LPR_T, NO_ATTR);
		 arithmeticExpression();
		 matchToken(RPR_T, NO_ATTR);

		 printf("PLATY: Primary arithmetic expression parsed\n");
		 break;

	 default:
		 printError();
		 break;
	 }

 }

 /*Function name: stringExpression
  Purpose: parse through stringExpression
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: primaryStringExpression(), stringExpression()
  Algorithm: start grammar and when down print parsed output*/
 void stringExpression() {

	 primaryStringExpression();
	 stringExpression();
	
	 printf("PLATY: String expression parsed\n");

 }

 /*Function name: primaryStringExpression
  Purpose: parse through primaryStringExpression
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: matchToken(), printError()
  Algorithm: if TKN = SVID_T/STR_T match token and start new grammar else throw error*/
 void primaryStringExpression() {

	 switch (lookahead.code) {
	 case SVID_T: case STR_T:
		 matchToken(lookahead.code, NO_ATTR);

		 printf("PLATY: Primary string expression parsed\n");
		 break;

	 default:
		 printError();
		 break;
	 }

 }

 /*Function name: stringExpressionPrime
  Purpose: parse through stringExpressionPrime
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: matchToken(), primaryStringExpression(), stringExpressionPrime()
  Algorithm: if TKN = SCC_OP_T match token and continue grammar*/
 void stringExpressionPrime() {

	 /*if TKN = SCC_OP_T match TKN continue grammar*/
	 if (lookahead.code == SCC_OP_T) {

		 matchToken(SCC_OP_T, NO_ATTR);
		 primaryStringExpression();
		 stringExpressionPrime();

	 }

 }

 /*Function name: conditionalExpression
  Purpose: parse through conditionalExpression
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: logicalOrExpression()
  Algorithm: continue grammar parsing and print ouput when done*/
 void conditionalExpression() {

	 logicalOrExpression();
	 printf("PLATY: Conditional expression parsed\n");

 }

 /*Function name: logicalOrExpression
  Purpose: parse through logicalOrExpression
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: logicalAndExpression(), logicalOrExpressionPrime()
  Algorithm: continue grammar parsing try to find AND / OR TKN*/
 void logicalOrExpression() {

	 logicalAndExpression();
	 logicalOrExpressionPrime();

 }

 /*Function name: logicalOrExpressionPrime
  Purpose: parse through logicalOrExpressionPrime
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: matchToken(), logicalAndExpression(), logicalOrExpressionPrime()
  Algorithm: if TKN = LOG_OP_T se if TKN is "OR" if so match token and continue parsing grammar*/
 void logicalOrExpressionPrime() {

	 /*if TKN = LOG_OP_T check if get_int = 1*/
	 if (lookahead.code == LOG_OP_T) {

		 /*if get_int = 1 match token continue grammar*/
		 if (lookahead.attribute.get_int == 1) {
			 matchToken(LOG_OP_T, lookahead.attribute.get_int);
			 logicalAndExpression();
			 logicalOrExpressionPrime();

			 printf("PLATY: Logical OR expression parsed\n");
		 }

	 }

 }

 /*Function name: logicalAndExpression
  Purpose: parse through logicalAndExpression
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: logicalAndExpression(), logicalOrExpressionPrime()
  Algorithm: continue parsing AND grammar*/
 void logicalAndExpression() {

	 relationalExpression();
	 logicalAndExpressionPrime();

 }

 /*Function name: logicalAndExpressionPrime
  Purpose: parse through logicalAndExpressionPrime
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: matchToken(), relationalExpression(), logicalAndExpressionPrime()
  Algorithm: if TKN = LOG_OP_T se if TKN is "AND" if so match token and continue parsing grammar*/
 void logicalAndExpressionPrime() {

	 /*if TKN = LOG_OP_T check if get_int = 0*/
	 if (lookahead.code == LOG_OP_T) {

		 /*if get_int = 0 match token continue grammar*/
		 if (lookahead.attribute.get_int == 0) {
			 matchToken(LOG_OP_T, lookahead.attribute.get_int);
			 relationalExpression();
			 logicalAndExpressionPrime();

			 printf("PLATY: Logical AND expression parsed\n");
		 }

	 }

 }

 /*Function name: relationalExpression
  Purpose: parse through relationalExpression
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: primary_a_relationalExpression(), operator_list(), primary_s_relationalExpression(), printError()
  Algorithm: if TKN = AVID_T/INL_T/FPL_T run primary_a_relationalExpression grammar else if TKN = SVID_T/STR_T run primary_s_relationalExpression grammar else run error*/
 void relationalExpression() {

	 switch (lookahead.code) {
	 case AVID_T: case INL_T: case FPL_T:
		 primary_a_relationalExpression();
		 operator_list();
		 primary_a_relationalExpression();
		 break;

	 case SVID_T: case STR_T:
		 primary_s_relationalExpression();
		 operator_list();
		 primary_s_relationalExpression();
		 break;

	 default:
		 printError();
		 break;
	 }

	 printf("PLATY: Relational expression parsed\n");
 }

 /*Function name: operator_list
  Purpose: parse through operator_list
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: matchToken(), printError()
  Algorithm: if TKN = REL_OP_T match token and end grammar else print error*/
 void operator_list() {

	 /*if TKN = REL_OP_T match TKN else call error*/
	 if (lookahead.code == REL_OP_T){
		 matchToken(REL_OP_T, lookahead.attribute.log_op);
	 }
	 else {
		 printError();
	 }

 }

 /*Function name: primary_a_relationalExpression
  Purpose: parse through primary_a_relationalExpression
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: matchToken(), printError()
  Algorithm: if TKN = AVID_T/INL_T/FPL_T match token and end grammar else print error*/
 void primary_a_relationalExpression() {

	 switch (lookahead.code) {
	 case AVID_T: case INL_T: case FPL_T:
		 matchToken(lookahead.code, NO_ATTR);
		 break;

	 default:
		 printError();
		 break;
	 }

	 printf("PLATY: Primary a_relational expression parsed\n");

 }

 /*Function name: primary_s_relationalExpression
  Purpose: parse through primary_s_relationalExpression
  Author: Asim Jasarevic
  History/Versions: 1.0, 13 Dec 2020
  Called functions: primaryStringExpression()
  Algorithm: continue grammar and print statment*/
 void primary_s_relationalExpression() {

	 primaryStringExpression();
	 printf("PLATY: Selection statement parsed\n");

 }


 /*Function name: getNextTKN
 Purpose: parse through getNextTKN
 Author: Asim Jasarevic
 History/Versions: 1.0, 13 Dec 2020
 Called functions: processToken()
 Algorithm: get next token that's not NLL_T*/
 void getNextTKN() {
	 /*while TKN = NLL_T get next TKN*/
		while (lookahead.code == NLL_T) {
			lookahead = processToken();
		}
 }
