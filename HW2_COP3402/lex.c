/*
Homework:
lex - HW2 PL/0 lexical analyzer

Author(s): Dan Le

Language: C only

To Compile:
	gcc -Wall -Wextra -std=c11 -O2 lex.c -o lex

To Execute (on Eustis):
	./lex input.txt

where:
	input.txt is the path to a text file holding a PL/0 source program

Notes:
	- Implements the lexical analyzer described in the homework
		instructions.
		The required header comment (continued)
	- Prints four sections to standard output: Source Program, Lexeme
		Table, Name Table and Token List.
	- Writes two files into the working directory: tokens.txt and
		nametable.txt.
	- Stops at the first lexical error, prints everything scanned before
		it, reports the error with its line and column, and exits with a
		non-zero status.
	- Exits with status 0 when the whole program scans without an error.
	- Tested on Eustis.

Class: COP 3402 - Systems Software

Instructor: Jie Lin, Ph.D.

Due Date: See Webcourses
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

void charAdd(char* x, char y) {

	while (*x++);
	*(x - 1) = y;
	*x = '\0';

}

int main(int argc, char* argv[]) {

	//char valid_operators[100][100] = {"+", "-", "*", "/", "==", "!=", "<", "<=", ">", ">=", "(", ")", ",", ";", ".", "=", ":=", "begin", "end", "if", "fi", "then", "while", "elihw", "do", "od", "odd", "call", "const", "var", "procedure", "write", "read", "else", NULL};
	char valid_operators[100][100] = { "+", "-", "*", "/", "=", "<", ">", "(", ")", ",", ";", ".", };
	char invalid_operators[100][100] = { "#", "$", "@", "?", "_", "'", "`", "[", "]", "{", "}", "\\", };

	int tokenAmount = 0;
	int c;
	int count = 0;
	int commentFlag = 0;
	int lexFlag = 0;
	int varTag = 0;

	char lexemeArray[1000][1000];
	char alreadyUsed[1000][1000];
	char nameTable[1000][1000];

	int nameTableIndex = 0;
	int AU_INDEX = 0;

	int tokenArray[1000];
	int printedArray[1000];
	int TKN_POINTER = 0;

	int row = 1;
	int col = 1;

	int NT_ROW[1000];
	int NT_COL[1000];

	
	if (argc == 1) {
		printf("Usage: ./lex <input file>");
		puts("");
		return 1;
	}
	else if (argc > 2){
		printf("Usage: ./lex <input file>");
		puts("");
		return 1;
	}
	

	

	FILE* file;
	file = fopen(argv[1], "r");
	char inputFile[100];
	char temp[100];

	if (file == NULL) {
		printf("Error: unable to open input file '%s'", argv[1]);
		puts("");
		return 1;
	}

	printf("Source Program:\n\n");

	char fileString[100] = "";

	while ((c = fgetc(file)) != EOF) {

		if (c != '\r' || c != '\n') {
			charAdd(fileString, c);

		}
		printf("%c", c);
		count = count + 1;

	}


	// EMERGENCY
	//charAdd(fileString, '\0');

	fclose(file);

	puts("");



	printf("Lexeme Table:\n\n");
	printf("lexeme		token\n");



	char character;
	int y = 0;
	int jump = 0;
	int spaceTag = 0;
	int varBar = 0;
	int already_used_flag = 0;
	int buffer = 0;
	int errorTag = 0;
	int punctSkip = 0;



	int lexIndex = 0;
	int errorLine = 0;
	char errorOneValue;
	char errorTwoValue[100];
	char errorThreeValue[100];
	char errorSixValue[100];
	char errorTenValue[100];

	int errorRow = 0;
	int errorCol = 0;
	int errorOneTrigger = 0;

	int commentRow = 0;
	int commentCol = 0;

	int shadowCol = 0;

	char characterHex[100];

	int netRow = 1;
	int netCol = 1;

	if (count == 0) {
		errorTag = 11;
	}

	char characterL[100];

	// ------------------------------------------------------------ MAIN SCANNING LOOP ------------------------------------------------------------

	for (int x = 0; x < count; x++) {


		punctSkip = 0;

		character = fileString[x];

		if (character == '\v') {
			errorTag = 10;
			strcpy(errorTenValue, "0B");
			errorRow = row;
			errorCol = col;
			break;
		}


		sprintf(characterHex, "0x%02X\n", character);
		sprintf(characterL, "%c%c", characterHex[strlen(characterHex) - 3], characterHex[strlen(characterHex) - 2]);

		if (strlen(characterHex) >= 6 || strcmp(characterL, "7F") == 0 || strcmp(characterL, "00") == 0 || strcmp(characterL, "1F") == 0) {
			errorTag = 10;
			sprintf(errorTenValue, "%c%c", characterHex[strlen(characterHex) - 3], characterHex[strlen(characterHex) - 2]);
			errorRow = row;
			errorCol = col;
			break;
		}

		if (strcmp(characterL, "0A") == 0) {
			netRow++;
			netCol = 1;
		}
		else {
			netCol++;
		}


		y = 0;
		jump = 0;
		spaceTag = 0;
		temp[0] = '\0';
		lexFlag = 0;
		already_used_flag = 0;
		if (commentFlag == 0) {
			if (errorOneTrigger == 0) {
				for (int z = 0; z < 12; z++) {
					if (character == *invalid_operators[z]) {
						errorTag = 1;
						errorOneValue = character;
						errorRow = row;
						errorCol = col;
						errorOneTrigger = 1;
						break;
					}
				}
			}
		}



		if (commentFlag == 0) {
			// Check for comment start
			if (fileString[x] == '/' && fileString[x + 1] == '*') {
				commentFlag = 1;
				commentRow = row;
				commentCol = col;
				if (fileString[x + 2] == '/') {
					errorTag = 7;
					errorOneTrigger = 1;
					errorRow = row;
					errorCol = col;

				}
				
			}

			else if ((fileString[x] == '*' && fileString[x + 1] == '/') && commentFlag == 0) {
				errorOneTrigger = 1;
				errorTag = 8;
				errorRow = row;
				if ((fileString[x + 2] != '\n' || fileString[x + 2] != '\r')) {
					errorCol = shadowCol + 2;
					if ((fileString[x + 2] == '*') && fileString[x + 3] != '/') {
						errorCol = col;
					}
				}
				else {
					errorCol = col;
				}
				
				break;
			}

			else {
				if (isalpha(character)) {
					lexFlag = 1;
					charAdd(temp, fileString[x]);
					while ((isalpha(fileString[x + y])) && !isspace(fileString[x + y]) && !ispunct(fileString[x + y]) && !iscntrl(fileString[x + y])) {
						y++;
						jump++;
						if (!ispunct(fileString[x + y]) && !isspace(fileString[x + y])) {
							charAdd(temp, fileString[x + y]);
						}
						if (y > 12) {
							errorTag = 2;
						}



					}

					if ((!iscntrl(fileString[x + y]) && !ispunct(fileString[x + y])) || fileString[x + y] == '\t') {


						col = col + jump;
						shadowCol = col;
					}
					else {
						if (ispunct(fileString[x + y])) {

						}
					}
				

					if ((strcmp(temp, "var")) == 0) {
						varBar = 1;
					}

					// Check for special cases

					if ((strcmp(temp, "begin") != 0) && (strcmp(temp, "end") != 0) && (strcmp(temp, "if") != 0) && (strcmp(temp, "fi") != 0) && (strcmp(temp, "then") != 0) && (strcmp(temp, "while") != 0) && (strcmp(temp, "elihw") != 0) && (strcmp(temp, "do") != 0) && (strcmp(temp, "od") != 0) && (strcmp(temp, "odd") != 0) && (strcmp(temp, "call") != 0) && (strcmp(temp, "const") != 0) && (strcmp(temp, "var") != 0) && (strcmp(temp, "procedure") != 0) && (strcmp(temp, "write") != 0) && (strcmp(temp, "read") != 0) && (strcmp(temp, "else") != 0)) {

						// Checks if entry is repeating or not

						for (int x = 0; x < AU_INDEX + 1; x++) {
							if ((strcmp(temp, alreadyUsed[x])) == 0) {
								already_used_flag = 1;
							}
						}

						if (already_used_flag == 0) {
							NT_ROW[AU_INDEX] = row;
							if (((isspace(fileString[x + y])) == 0)) {
								NT_COL[AU_INDEX] = col;
								
								col++;
							}
							else {
								
								if (fileString[x + 1] == '\n') {
									NT_COL[AU_INDEX] = col;
								}
								else {
									NT_COL[AU_INDEX] = col - jump;
								}
								
							}
							strcpy(alreadyUsed[AU_INDEX], temp);
							AU_INDEX++;
							varBar = 0;
						}

					}

					if (errorTag == 2 && errorOneTrigger == 0) {
						strcpy(errorTwoValue, temp);
						errorRow = row;
						errorCol = col - 1;
						errorOneTrigger = 1;
					}

					x = x + jump - 1;
				}
				else if (isdigit(character)) {
					lexFlag = 1;
					charAdd(temp, fileString[x]);
					if (isdigit(character)) {
						while ((isdigit(fileString[x + y])) && !ispunct(fileString[x + y]) && !isspace(fileString[x + y] && !iscntrl(fileString[x + y] && !isalpha(fileString[x + y])))) {
							y++;
							jump++;


							if (!iscntrl(fileString[x + y]) && !ispunct(fileString[x + y])) {
								charAdd(temp, fileString[x + y]);
							}
							if (isalpha(fileString[x + y])) {
								errorTag = 6;
								while ((isalpha(fileString[x + y]))) {
									y++;
									if (!isspace(fileString[x + y]) && !ispunct(fileString[x + y])) {
										charAdd(temp, fileString[x + y]);
									}
								}
								errorOneTrigger = 2;
							}
							if ((y > 6) && errorOneTrigger == 0) {
								errorTag = 3;
							}
						}


						if (errorTag == 6 && errorOneTrigger == 0 || errorOneTrigger == 2) {
							strcpy(errorSixValue, temp);
							errorRow = row;
							errorCol = col;
							errorOneTrigger = 1;
						}

						if (!iscntrl(fileString[x + y]) && !ispunct(fileString[x + y])) {

						}
						col = col + jump;
						shadowCol = col;

						x = x + jump - 1;

						if (errorTag == 3 && errorOneTrigger == 0) {
							strcpy(errorThreeValue, temp);


							errorRow = row;
							errorCol = col - y;
							errorOneTrigger = 1;
						}


					}

				}
				else if (isspace(character)) {


					if (character == '\n' && character != '\r' && !character == '\0' && strcmp(characterL, "0A") == 0) {

						row++;
						jump = 0;
						col = 1;
						shadowCol = 1;
					}
					else {
						if (character != '\r') {
							col++;
							shadowCol = col;
						}

					}

				}
				else {

					if (character == ':' && fileString[x + 1] != '=') {
						errorTag = 4;
						errorRow = row;
						errorCol = col;
					}

					if (character == '!' && (fileString[x + 1] != '=' || fileString[x + 1] == '!')) {
						errorTag = 5;
						errorRow = row;
						errorCol = col;
						if (fileString[x + 1] == '!') {
							col--;
						}
					}

					if (ispunct(fileString[x])) {
						lexFlag = 1;
						//printf("%c", character);

						if ((((fileString[x] == '=') || (fileString[x] == '!')) && (fileString[x + 1] == '='))) {
							//col++;
							punctSkip = 1;
						}

						if ((((fileString[x] == '<') || (fileString[x] == '>') || (fileString[x] == ':')) && (fileString[x + 1] == '='))) {
							//printf("asd");
							punctSkip = 1;
						}



						while (ispunct(fileString[x + y]) && punctSkip == 0) {
							if (fileString[x + 1] == '\t') {
								col++;
							}
							if (errorOneTrigger == 0) {
								for (int p = 0; p < 12; p++) {
									if (fileString[x + y] == *invalid_operators[p]) {
										errorTag = 1;
										errorOneTrigger = 1;
										errorOneValue = fileString[x + y];
										errorRow = row;
										errorCol = shadowCol + jump + 1;
									}
								}
							}
							if (errorOneTrigger == 0) {
								charAdd(temp, fileString[x + y]);
							}

							y++;
							jump++;

							if (fileString[x] != '<' && fileString[x+1] != '=') {
								break;
							}
							if (fileString[x] == '<' && (fileString[x + 1] == '>' || fileString[x + 1] == '/')) {
								break;
							}
							if ((fileString[x] == '+' || fileString[x] == '-') && fileString[x + 1] == '=') {
								break;
							}

							

						}
						if (punctSkip == 1) {
							y = y + 2;
							jump = jump + 2;
							charAdd(temp, fileString[x]);
							charAdd(temp, fileString[x + 1]);
							punctSkip = 0;
						}
						else if (punctSkip == 2) {
							y++;
							jump = jump + 1;
							charAdd(temp, fileString[x]);
							punctSkip = 0;
						}

						if (!iscntrl(fileString[x + y])) {
							col = col + jump;
							shadowCol = col;
						}
						x = x + jump - 1;
					}
				}
			}




		}

		// IF DURING COMMENT

		else {
			if (fileString[x] == '/' && fileString[x + 1] == '*' && errorOneTrigger == 0) {
				errorTag = 9;
				errorRow = row;
				errorCol = shadowCol - 1;
				errorOneTrigger = 1;
				break;
			}



			if (fileString[x] == '*' && fileString[x + 1] == '/' && errorOneTrigger == 0) {
				x = x + 2;
				commentFlag = 0;
				commentRow = 0;
				commentCol = 0;
				if (fileString[x + 1] == '\r') {
					row++;
				}
				if (iscntrl(fileString[x + 1]) == 0) {
					x--;
				}
			}


		}
		if (commentFlag == 0 && spaceTag == 0) {
			//printf("%c", fileString[x]);
		}

		if (lexFlag == 1) {
			strcpy(lexemeArray[lexIndex], temp);
			lexIndex++;
		}

		shadowCol++;
		netCol = netCol + jump;
	}


	char onFirstUse[1000][1000];
	int alreadyPrinted = 0;
	int INDEX_2 = 0;


	for (int x = 0; x < lexIndex; x++) {


		alreadyPrinted = 0;


		// ------------------------------------ Error Catching ------------------------------------------------------------------------ 

		if (errorTag == 2 && strcmp(lexemeArray[x], errorTwoValue) == 0) {
			break;
		}

		if (errorTag == 3 && strcmp(lexemeArray[x], errorThreeValue) == 0) {
			break;
		}

		if (errorTag == 4 && strcmp(lexemeArray[x], ":") == 0) {
			break;
		}

		if (errorTag == 5 && strcmp(lexemeArray[x], "!") == 0) {
			break;
		}

		if (errorTag == 6 && strcmp(lexemeArray[x], errorSixValue) == 0) {
			break;
		}

		if (errorTag == 1 && *lexemeArray[x] == errorOneValue) {
			//printf("\n");
		}
		else {
			printf("%s		", lexemeArray[x]);
		}


		// ------------------------------------ LEXEME TO TOKEN ASSIGNING ------------------------------------------------------------------------  

		if (strcmp(lexemeArray[x], "+") == 0) {
			tokenArray[TKN_POINTER] = 3;
			printedArray[x] = 3;
		}

		else if (strcmp(lexemeArray[x], "-") == 0) {
			tokenArray[TKN_POINTER] = 4;
			printedArray[x] = 4;
		}

		else if (strcmp(lexemeArray[x], "*") == 0) {
			tokenArray[TKN_POINTER] = 5;
			printedArray[x] = 5;
		}

		else if (strcmp(lexemeArray[x], "/") == 0) {
			tokenArray[TKN_POINTER] = 6;
			printedArray[x] = 6;
		}

		else if (strcmp(lexemeArray[x], "==") == 0) { 
			tokenArray[TKN_POINTER] = 7;
			printedArray[x] = 7;
		}

		else if (strcmp(lexemeArray[x], "!=") == 0) { 
			tokenArray[TKN_POINTER] = 8;
			printedArray[x] = 8;
		}

		else if (strcmp(lexemeArray[x], "<") == 0) {
			tokenArray[TKN_POINTER] = 9;
			printedArray[x] = 9;
		}

		else if (strcmp(lexemeArray[x], "<=") == 0) { 
			tokenArray[TKN_POINTER] = 10;
			printedArray[x] = 10;
		}

		else if (strcmp(lexemeArray[x], ">") == 0) {
			tokenArray[TKN_POINTER] = 11;
			printedArray[x] = 11;
		}

		else if (strcmp(lexemeArray[x], ">=") == 0) { 
			tokenArray[TKN_POINTER] = 12;
			printedArray[x] = 12;
		}

		else if (strcmp(lexemeArray[x], "(") == 0) {
			tokenArray[TKN_POINTER] = 13;
			printedArray[x] = 13;
		}

		else if (strcmp(lexemeArray[x], ")") == 0) {
			tokenArray[TKN_POINTER] = 14;
			printedArray[x] = 14;
		}

		else if (strcmp(lexemeArray[x], ",") == 0) {
			tokenArray[TKN_POINTER] = 15;
			printedArray[x] = 15;
		}

		else if (strcmp(lexemeArray[x], ";") == 0) {
			tokenArray[TKN_POINTER] = 16;
			printedArray[x] = 16;
		}

		else if (strcmp(lexemeArray[x], ".") == 0) {
			tokenArray[TKN_POINTER] = 17;
			printedArray[x] = 17;
		}

		else if (strcmp(lexemeArray[x], "=") == 0) {
			tokenArray[TKN_POINTER] = 18;
			printedArray[x] = 18;
		}

		else if (strcmp(lexemeArray[x], ":=") == 0) {
			tokenArray[TKN_POINTER] = 19;
			printedArray[x] = 19;
		}

		else if (strcmp(lexemeArray[x], "begin") == 0) {
			tokenArray[TKN_POINTER] = 20;
			printedArray[x] = 20;
		}

		else if (strcmp(lexemeArray[x], "end") == 0) {
			tokenArray[TKN_POINTER] = 21;
			printedArray[x] = 21;
		}

		else if (strcmp(lexemeArray[x], "if") == 0) {
			tokenArray[TKN_POINTER] = 22;
			printedArray[x] = 22;
		}

		else if (strcmp(lexemeArray[x], "fi") == 0) {
			tokenArray[TKN_POINTER] = 23;
			printedArray[x] = 23;
		}

		else if (strcmp(lexemeArray[x], "then") == 0) {
			tokenArray[TKN_POINTER] = 24;
			printedArray[x] = 24;
		}

		else if (strcmp(lexemeArray[x], "while") == 0) {
			tokenArray[TKN_POINTER] = 25;
			printedArray[x] = 25;
		}

		else if (strcmp(lexemeArray[x], "elihw") == 0) {
			tokenArray[TKN_POINTER] = 26;
			printedArray[x] = 26;
		}

		else if (strcmp(lexemeArray[x], "do") == 0) {
			tokenArray[TKN_POINTER] = 27;
			printedArray[x] = 27;
		}

		else if (strcmp(lexemeArray[x], "od") == 0) {
			tokenArray[TKN_POINTER] = 28;
			printedArray[x] = 28;
		}

		else if (strcmp(lexemeArray[x], "odd") == 0) {
			tokenArray[TKN_POINTER] = 29;
			printedArray[x] = 29;
		}

		else if (strcmp(lexemeArray[x], "call") == 0) {
			tokenArray[TKN_POINTER] = 30;
			printedArray[x] = 30;
		}

		else if (strcmp(lexemeArray[x], "const") == 0) {
			tokenArray[TKN_POINTER] = 31;
			printedArray[x] = 31;
		}

		else if (strcmp(lexemeArray[x], "var") == 0) {
			tokenArray[TKN_POINTER] = 32;
			printedArray[x] = 32;
			varTag = 1;
		}

		else if (strcmp(lexemeArray[x], "procedure") == 0) {
			tokenArray[TKN_POINTER] = 33;
			printedArray[x] = 33;
		}

		else if (strcmp(lexemeArray[x], "write") == 0) {
			tokenArray[TKN_POINTER] = 34;
			printedArray[x] = 34;
		}

		else if (strcmp(lexemeArray[x], "read") == 0) {
			tokenArray[TKN_POINTER] = 35;
			printedArray[x] = 35;
		}

		else if (strcmp(lexemeArray[x], "else") == 0) {
			tokenArray[TKN_POINTER] = 36;
			printedArray[x] = 36;
		}
		else {
			if (strspn(lexemeArray[x], "0123456789") == 0) {

				if (isalnum(*lexemeArray[x]) == 0) {
					errorTag = 1;
					break;
				}

				tokenArray[TKN_POINTER] = 1;
				printedArray[x] = 1;

				for (int y = 0; y < INDEX_2 + 1; y++) {
					if ((strcmp(lexemeArray[x], onFirstUse[y])) == 0) {
						alreadyPrinted = 1;
					}
				}

				if (alreadyPrinted == 0) {
					strcpy(nameTable[nameTableIndex], lexemeArray[x]);
					nameTableIndex++;
					varTag = 0;
					strcpy(onFirstUse[INDEX_2], lexemeArray[x]);
					INDEX_2++;

					TKN_POINTER++;
					tokenArray[TKN_POINTER] = (nameTableIndex - 1);
				}
				else {
					//nameTableIndex = 1;
					for (int z = 0; z < nameTableIndex + 1; z++) {
						if (strcmp(lexemeArray[x], nameTable[z]) == 0) {
							//printf("WOAH");
							TKN_POINTER++;
							tokenArray[TKN_POINTER] = z;
							break;
						}
					}
				}
			}
			else {



				tokenArray[TKN_POINTER] = 2;
				printedArray[x] = 2;
				TKN_POINTER++;
				tokenArray[TKN_POINTER] = atoi(lexemeArray[x]);

			}
		}
		printf("%d\n", printedArray[x]);
		TKN_POINTER++;



	}

	if (commentFlag == 1 && errorOneTrigger == 0) {
		errorTag = 7;
		errorRow = commentRow;
		errorCol = commentCol;
		errorOneTrigger = 1;
	}

	if (TKN_POINTER == 0 && errorOneTrigger == 0) {
		errorTag = 11;
	}

	puts("");

	// ------------------------------------  PRINTING OUT NAME TABLES AND TOKENS ------------------------------------------------------------------------ 
	printf("Name Table:\n\n");
	printf("index	name		line	column");
	puts("");
	for (int x = 0; x < nameTableIndex; x++) {
		printf("%d	%s		%d	%d", x, nameTable[x], NT_ROW[x], NT_COL[x]);
		puts("");
	}
	puts("");
	puts("");

	printf("Token List:\n\n");
	for (int x = 0; x < TKN_POINTER; x++) {
		printf("%d ", tokenArray[x]);
	}

	puts("");

	// ------------------------------------ ERROR HANDLING ------------------------------------------------------------------------  

	if (errorTag >= 1) {
		puts("");
		if (errorTag == 1) {
			printf("Error %d at line %d, column %d: invalid character '%c'", errorTag, errorRow, errorCol, errorOneValue);
		}
		else if (errorTag == 2) {
			printf("Error %d at line %d, column %d: identifier too long '%s'", errorTag, errorRow, errorCol, errorTwoValue);
		}
		else if (errorTag == 3) {
			errorThreeValue[strlen(errorThreeValue) - 1] = '\0';
			printf("Error %d at line %d, column %d: number too long '%s'", errorTag, errorRow, errorCol, errorThreeValue);
		}
		else if (errorTag == 4) {
			printf("Error %d at line %d, column %d: ':' must be followed by '='", errorTag, errorRow, errorCol);
		}
		else if (errorTag == 5) {
			printf("Error %d at line %d, column %d: '!' must be followed by '='", errorTag, errorRow, errorCol);
		}
		else if (errorTag == 6) {
			printf("Error %d at line %d, column %d: number followed by a letter '%s'", errorTag, errorRow, errorCol, errorSixValue);
		}
		else if (errorTag == 7) {
			printf("Error %d at line %d, column %d: comment is not closed before end of file", errorTag, errorRow, errorCol);
		}
		else if (errorTag == 8) {
			printf("Error %d at line %d, column %d: '*/' without a matching '/*'", errorTag, errorRow, errorCol);
		}
		else if (errorTag == 9) {
			printf("Error %d at line %d, column %d: '/*' inside a comment", errorTag, errorRow, errorCol);
		}
		else if (errorTag == 10) {
			printf("Error %d at line %d, column %d: byte 0x%s is not part of this language", errorTag, errorRow, errorCol, errorTenValue);
		}
		else if (errorTag == 11) {
			printf("Error %d at line 1, column 1: no tokens in the source program", errorTag);
		}
		puts("");
		//return 1;
	}


	// ------------------------------------ WRITING TO OTHER FILES ------------------------------------------------------------------------  

	file = fopen("tokens.txt", "w");

	int next = 0;

	for (int x = 0; x < TKN_POINTER; x++) {
		fprintf(file, "%d ", tokenArray[x]);
		if (tokenArray[x] == 1 || tokenArray[x] == 2) {
			fprintf(file, "%d", tokenArray[x + 1]);
			x++;
		}
		fprintf(file, "\n");

	}

	fclose(file);

	file = fopen("nametable.txt", "w");

	for (int x = 0; x < nameTableIndex; x++) {
		fprintf(file, "%d %s %d %d", x, nameTable[x], NT_ROW[x], NT_COL[x]);
		fprintf(file, "\n");
	}

	if (errorTag >= 1) {
		return 1;
	}
}