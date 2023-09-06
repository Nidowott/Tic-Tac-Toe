#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#else
	#include <time.h>
#endif

/* Constants */
#define NOMATCHPOS 9

/* Game settings struct (these variables are not updated throughout the game) */
struct gameSetup {
	int player;
	char playerMark;
	int difficulty;
	int ai;	
};

/* Game variables struct (these variables are updated throughout the game) */
struct gameVariables {
	char positions[9];
	char aiPositions[9];
	int turn;
	int playerLastMove;
	int aiLastMove;
};

/* Reads and returns user input as lowercase */
char* returnString(char* input) {
	int numRead = 0;
	int next;
	while (1) {
		next = fgetc(stdin);
		if (next == EOF && numRead == 0) {
			input = "error";
			return input;
			}
		if (next == '\n' || next == EOF) {
			input[numRead] = '\0';
			break;
			}
		input[numRead++] = tolower(next);
		}
	return input;
}

/* Checks if the User input is an integer */
int checkInt(char* input) {
	int next = 0;
	while(1) {
		if (isdigit(input[next] == 0)) {
			return 0;
			}
		if (input[next] == '\0') {
			break;
			}
		next++;
	}
	return 1;
}

/* Player input setup loop */
struct gameSetup setup() {
	int players;
	struct gameSetup setup;
	setup.player = 1;
	setup.playerMark = 'x';
	setup.difficulty = 0;
	setup.ai = 0;
	while (1) {
		char* input = (char*) malloc(sizeof(char) * 51);
		printf("'1' or '2' players?\n");
		input = returnString(input);
		if ((strcmp(input, "1") == 0) || (strcmp(input, "2") == 0)) {
			players = atoi(input);
			free(input);
			break;
		} else {
			printf("Invalid choice!\n");
			free(input);
		}
	}
	while (players < 2) {
		char* input = (char*) malloc(sizeof(char) * 51);
		printf("SELECT PLAYER\n'x' for player 1, 'o' for player 2\n");
		input = returnString(input);
		if (strcmp(input, "o") == 0) {
			setup.player = 2;
			setup.playerMark = 'o';;
			setup.ai = 1;
			free(input);
			break;
		} else if (strcmp(input, "x") == 0) {
			setup.ai = 2;
			free(input);
			break;
		} else {
			printf("Invalid choice!\n");
			free(input);
		}
	}
	while (players < 2) {
		char* input = (char*) malloc(sizeof(char) * 51);
		printf("SELECT DIFFICULTY\n'easy' or 'hard'\n");
		input = returnString(input);
		if (strcmp(input, "hard") == 0) {
			setup.difficulty = 1;
			free(input);
			break;
		} else if (strcmp(input, "easy") == 0) {
			free(input);
			break;
		} else {
			printf("Invalid choice!\n");
			free(input);
		}
	}
	return setup;
}

/* Returns 0 for no win 1 for p1 win 2 for p2 win */
int checkWin(char positions[9], int turnPlayer, int ai) {
	if ((positions[0] == positions[1] && positions[1] == positions[2]) ||
	(positions[3] == positions[4] && positions[4] == positions[5]) ||
	(positions[6] == positions[7] && positions[7] == positions[8]) ||
	(positions[0] == positions[3] && positions[3] == positions[6]) ||
	(positions[1] == positions[4] && positions[4] == positions[7]) ||
	(positions[2] == positions[5] && positions[5] == positions[8]) ||
	(positions[0] == positions[4] && positions[4] == positions[8]) ||
	(positions[2] == positions[4] && positions[4] == positions[6])) {
		printf("Player %d has won!\n", turnPlayer);
		if (ai > 0) {
			if (ai == turnPlayer) {
				printf("You lose!\n");
			} else {
				printf("You beat the ai!\n");
			}
		}
		return 1;
	} else {
		return 0;
	}
}

/* Checks for positions 1 turn away from winning */
int checkAlmostWon(char positions[9]) {
	if ((positions[1] == positions[2] && (positions[0] == '1')) ||
	(positions[3] == positions[6] && (positions[0] == '1')) ||
	(positions[4] == positions[8] && (positions[0] == '1'))) {
		return 0;
	} else if ((positions[0] == positions[2] && (positions[1] == '2')) ||
	(positions[4] == positions[7] && (positions[1] == '2'))) {
		return 1;
	} else if ((positions[0] == positions[1] && (positions[2] == '3')) ||
	(positions[4] == positions[6] && (positions[2] == '3')) ||
	(positions[5] == positions[8] && (positions[2] == '3'))) {
		return 2;
	} else if ((positions[0] == positions[6] && (positions[3] == '4')) ||
	(positions[4] == positions[5] && (positions[3] == '4'))) {
		return 3;
	} else if ((positions[0] == positions[8] && (positions[4] == '5')) ||
	(positions[1] == positions[7] && (positions[4] == '5')) ||
	(positions[2] == positions[6] && (positions[4] == '5')) ||
	(positions[3] == positions[5] && (positions[4] == '5'))) {
		return 4;
	} else if ((positions[2] == positions[8] && (positions[5] == '6')) ||
	(positions[3] == positions[4] && (positions[5] == '6'))) {
		return 5;
	} else if ((positions[0] == positions[3] && (positions[6] == '7')) ||
	(positions[2] == positions[4] && (positions[6] == '7')) ||
	(positions[7] == positions[8] && (positions[6] == '7'))) {
		return 6;
	} else if ((positions[1] == positions[4] && (positions[7] == '8')) ||
	(positions[6] == positions[8] && (positions[7] == '8'))) {
		return 7;
	} else if ((positions[0] == positions[4] && (positions[8] == '9')) ||
	(positions[2] == positions[5] && (positions[8] == '9')) ||
	(positions[6] == positions[7] && (positions[8] == '9'))) {
		return 8;
	} else {
		return NOMATCHPOS;
	}		
}

/* Picks random number from 0-8 and returns it provided the square is empty. */
int chooseRandomPosition(char positions[9]) {
	while(1) {
		int position = rand() % 9;
		if (positions[position] != 'x' && positions[position] != 'o') {
			return position;
		}
	}
}

/* Default hard ai move code, chooses a space next to the ai's mark if possible, otherwise returns a random position */
int aiStalemate(char positions[9], int playerLastMove) {
	int adjacentPositions[9][8] = {
		{1, 3, 4},     // 0
		{0, 2, 3, 4, 5}, // 1
		{1, 4, 5},     // 2
		{0, 1, 4, 6, 7}, // 3
		{0, 1, 2, 3, 5, 6, 7, 8}, // 4
		{1, 2, 4, 7, 8}, //  5
		{3, 4, 7},     // 6
		{3, 4, 5, 6, 8}, // 7
		{4, 5, 7}      // 8
	};
	char playerMark = positions[playerLastMove];
	char aiMark = 'x';
	if (playerMark == 'x') {
		aiMark = 'o';
	}
	for (int i = 0; i < 9; i++) {
		if (positions[i] == aiMark) {
			for (int j = 0; j < 8; j++) {
				if (positions[adjacentPositions[i][j]] != aiMark && positions[adjacentPositions[i][j]] != playerMark) {
					return adjacentPositions[i][j];
				}
			}
		}
	}
	return chooseRandomPosition(positions);
}

/* Returns winning move if possible, otherwise a move to block a win. Returns NOMATCHPOS if grid is not in a winnable state */
int aiGetPreventWin(char positions[9], char aiPositions[9]) {
	int playerWinPos = checkAlmostWon(positions);
	int aiWinPos = checkAlmostWon(positions);
	if (aiWinPos != NOMATCHPOS) {
		return aiWinPos;
	}
	return playerWinPos;
}

/* returns a random corner position */
int aiTurn0() {
	int corners[4] = { 0, 2, 6, 8 };
	int position = rand() % 4;
	return corners[position];
}

/* Returns the middle space if free, otherwise stalemate code */
int aiTurn1 (char positions[9], int playerLastMove) {
	if (positions[4] == '5') {
		return 4;
	}
	return aiStalemate(positions, playerLastMove);
}

/* Returns a corner position based on the ai and player's first moves */
int aiTurn2(char positions[9], int playerLastMove, int aiLastMove) {
	if (playerLastMove == 4) {
		return aiStalemate(positions, playerLastMove);
	}
	switch (aiLastMove)
	{
		case 0:
			if (playerLastMove == 8) {
				return 2 + rand() % 2 * 4; // returns 2 or 6 (corners)
			} else if (playerLastMove == 1 || playerLastMove == 2 || playerLastMove == 5) {
				return 6;
			} else {
				return 2;
			}
		case 2:
			if (playerLastMove == 6) {
				return rand() % 2 * 8; // returns 0 or 8 (corners)
			} else if (playerLastMove == 0 || playerLastMove == 1 || playerLastMove == 3) {
				return 8;
			} else {
				return 0;
			}
		case 6:
			if (playerLastMove == 2) {
				return rand() % 2 * 8; // returns 0 or 8 (corners)
			} else if (playerLastMove == 0 || playerLastMove == 1 || playerLastMove == 3) {
				return 8;
			} else {
				return 0;
			}
		case 8:
			if (playerLastMove == 0) {
				return 2 + rand() % 2 * 4; // returns 2 or 6 (corners)
			} else if (playerLastMove == 1 || playerLastMove == 2 || playerLastMove == 5) {
				return 6;
			} else {
				return 2;
			}
		default:
			return aiStalemate(positions, playerLastMove); // for easy mode when ai did not choose corner turn 0
	}
}

/* Used in certain cases where the game has not been won yet, returns a free corner (should only be one free on hard mode) */
int aiTurn4(char positions[9], int playerLastMove, int aiLastMove) {
	if (positions[0] == 'o' || positions[2] || 'o' || positions[6] == 'o' || positions[8] == 'o') { // case for player starting in a corner
		if (positions[0] == '1') {
			return 0;
		} else if (positions[2] == '3') {
			return 2;
		} else if (positions[6] == '7') {
			return 6;
		} else if (positions[8] == '9') {
			return 8;
		}
	} else if ((aiLastMove == 2 && playerLastMove == 1) || (aiLastMove == 6 && playerLastMove == 3)) {
		return 4 + rand() % 2 * 4; // returns 4 or 8
	} else if ((aiLastMove == 0 && playerLastMove == 1) || (aiLastMove == 8 && playerLastMove == 5)) {
		return 4 + rand() % 2 * 2; // returns 4 or 6
	} else if ((aiLastMove == 0 && playerLastMove == 3) || (aiLastMove == 8 && playerLastMove == 7)) {
		return 2 + rand() % 2 * 2; // returns 4 or 2
	} else if ((aiLastMove == 2 && playerLastMove == 5) || (aiLastMove == 6 && playerLastMove == 7)) {
		return rand() % 2 * 4; // returns 4 or 0
	} else {
		return aiStalemate(positions, playerLastMove); // for easy mode when the game state does not meet any of the cases
	}
}

/* Returns the method for the correct move based on the turn */
int chooseMoveTurn(struct gameVariables variables) {
	int position = aiGetPreventWin(variables.positions, variables.aiPositions);
	if (position != NOMATCHPOS) {
		return position;
	}
	switch(variables.turn)
	{
		case 0:
			position = aiTurn0();
			break;
		case 1:
			position = aiTurn1(variables.positions, variables.playerLastMove);
			break;
		case 2:
			position = aiTurn2(variables.positions, variables.playerLastMove, variables.aiLastMove);
			break;
		case 4:
			position = aiTurn4(variables.positions, variables.playerLastMove, variables.aiLastMove);
			break;
		default:
			position = aiStalemate(variables.positions, variables.playerLastMove);	
	}
	return position;
}

/* Handler for ai move based on difficulty */
int aiTurn(struct gameVariables variables, int difficulty) {
	printf("ai is thinking...\n");
	int goodMoveCheck = rand() % 4;
	if (difficulty == 1 || goodMoveCheck == 0) {
		return chooseMoveTurn(variables);
	}
	int position = chooseRandomPosition(variables.positions);
	if (position != NOMATCHPOS) {
		return position;
	}
}

/* Loop for player move input */
int playerTurn(char positions[9]) {
	int position = 0;
	while(1) {
		char* input = (char*) malloc(sizeof(char) * 51);
		printf("Choose a position on the grid (1-9):\n");
		input = returnString(input);
		if (checkInt(input) > 0) {
			position = atoi(input);
		}
		if (position > 0 && position < 10 && positions[position-1] != 'x' && positions[position-1] != 'o') {
			free(input);
			return position - 1;
		}
		free(input);
		printf("Invalid choice!\n");
	}
}

/* Prints the game grid from the positions array */
void printGrid(char positions[9]) {
	printf("Current game state:\n"); // fixes printing bug for some reason
	printf("\n %c | %c | %c \n", positions[0], positions[1], positions[2]);
	printf("---+---+---\n");
	printf(" %c | %c | %c \n", positions[3], positions[4], positions[5]);
	printf("---+---+---\n");
	printf(" %c | %c | %c \n", positions[6], positions[7], positions[8]);
}

/* Main game loop, handles both 1 player and 2 player games */
void gameLoop(struct gameSetup settings) {
	struct gameVariables variables = { "123456789", "123456789", 0, 9, 9 };
	if (settings.ai != 0) {
		printf("\nYou are player %d (%c).\n", settings.player, settings.playerMark);
	} else {
		printf("2 player game!\n");
	}
	printGrid(variables.positions);
	while(1) {
		printf("\nPlayer 1's turn.\n");
		if (settings.ai == 1) {
			variables.aiLastMove = aiTurn(variables, settings.difficulty);
			variables.positions[variables.aiLastMove] = 'x';
			variables.aiPositions[variables.aiLastMove] = 'x';
		} else {
			variables.playerLastMove = playerTurn(variables.positions);
			variables.positions[variables.playerLastMove] = 'x';				
		}
		printGrid(variables.positions);
		if (checkWin(variables.positions, 1, settings.ai) == 1) {
			break;
		}
		if (variables.turn++ == 8) {
			printf("Draw!\n");
			break;
		}
		printf("\nPlayer 2's turn.\n");
		if (settings.ai == 2) {
			variables.aiLastMove = aiTurn(variables, settings.difficulty);
			variables.positions[variables.aiLastMove] = 'o';
			variables.aiPositions[variables.aiLastMove] = 'o';
		} else {
			variables.playerLastMove = playerTurn(variables.positions);
			variables.positions[variables.playerLastMove] = 'o';
		}
		printGrid(variables.positions);
		if (checkWin(variables.positions, 2, settings.ai) == 1) {
			break;
		}
		variables.turn++;
	}
}

/* Loop to restart the game */
int playAgain() {
	while (1) {
		char* input = (char*) malloc(sizeof(char) * 51);
		printf("PLAY AGAIN? 'y' or 'n'\n");
		input = returnString(input);
		if (strcmp(input, "y") == 0) {
			free(input);
			return 1;
		} else if (strcmp(input, "n") == 0) {
			free(input);
			return 0;
		} else {
			printf("Invalid choice!\n");
			free(input);
		}
	}
}

/* Main */
int main() {
	srand(time(NULL));
	printf("WELCOME TO TIC-TAC-TOE!\n");
	while(1) {
		struct gameSetup settings = setup();
		gameLoop(settings);
		if (playAgain() == 0) {
			break;
		}
	}
	printf("Thanks for playing!\n");
	return 0;
}