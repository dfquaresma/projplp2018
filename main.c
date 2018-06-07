/*sudo apt-get install ncurses-dev*/
/*gcc <file-name>.c -lncurses -o <executable-name>*/
#include "player.h" // Also includes display, board and util recursively.

#define GAME_INTRODUCTION_STATE 0
#define MENU_STATE 1
#define DIFFICULTY_STATE 2
#define INSTRUCTION_STATE 3
#define START_GAME_STATE 4
#define PLAY_GAME_STATE 5
#define WIN_STATE 6
#define LOSE_STATE 7

void ncursesInit();
void ncursesEnd();
int getRandomIntegerInRange(int min,int max);
bool checkLoseCondition();
bool checkWinCondition();
bool usingStaticInstructionScreen = false;

int main() {
	ncursesInit();
	long long int timeSinceLastGameBoardDecrease;
	long long int lastBotsPositionUpdateTime;
	int gameState = GAME_INTRODUCTION_STATE;

	while(!userEscAction){
		updateNextUserAction();

		switch(gameState) {
			case GAME_INTRODUCTION_STATE:
				showMainGameIntroduction();
				gameState = MENU_STATE;
			break;

			case MENU_STATE:
				if(downMovement){
					usingStaticInstructionScreen = true;
					showGameIntroductionStaticInstructions();
				} else if(usingStaticInstructionScreen && userEnterAction){
					showGameInstructions();
					gameState = INSTRUCTION_STATE;
				} else if(usingStaticInstructionScreen && upMovement) {
					usingStaticInstructionScreen = false;
					showGameIntroductionStaticStart();
				} else if(!usingStaticInstructionScreen) {
					if(userEnterAction){
						gameState = DIFFICULTY_STATE;
					}
				}
			break;
			
			case INSTRUCTION_STATE:
				usingStaticInstructionScreen = false;
					if (userEnterAction) {
						clear();
						showMainGameIntroduction();
						gameState = MENU_STATE;
					}
			break;

			case DIFFICULTY_STATE:
				int difficultyOption = 0;
				showGameDifficultyOptions0();
				if(downMovement){
					difficultyOption = (difficultyOption + 1) % 3;
					switch (difficultyOption) {
					case 0:
						showGameDifficultyOptions0();
					break;
					
					case 1:
						showGameDifficultyOptions2();
					break;
					
					case 2:
						showGameDifficultyOptions2();
					break;
					}
				
				} else if (upMovement) {
					difficultyOption = difficultyOption - 1;
					if (difficultyOption < 0) {
						difficultyOption = 0;
					}
					switch (difficultyOption) {
					case 0:
						showGameDifficultyOptions0();
					break;
					
					case 1:
						showGameDifficultyOptions2();
					break;
					
					case 2:
						showGameDifficultyOptions2();
					break;
					}
				
				} else if(userEnterAction){
					switch (difficultyOption) {
					case 0:
						PLAYERS_NUMBER = 30;
					break;
					
					case 1:
						PLAYERS_NUMBER = 50;
					break;
					
					case 2:
						PLAYERS_NUMBER = 100;
					break;
					}
					
					gameState = START_GAME_STATE;
				} 
			break;

			case START_GAME_STATE:
				cataclysm = false;
				timeSinceLastGameBoardDecrease = getCurrentTimestamp();
				lastBotsPositionUpdateTime = getCurrentTimestamp();
				createPlayers();
				settingGameBoard();
				delay(60);
				gameState = PLAY_GAME_STATE;
			break;

			case PLAY_GAME_STATE:
				moveUser();
				moveBots(&lastBotsPositionUpdateTime);
				playersCollision();
				drawAlivePlayersNumber();
				drawPlayers();
				decreaseGameBoardByInterval(&timeSinceLastGameBoardDecrease);
				if (checkWinCondition())
					gameState = WIN_STATE;
				if (checkLoseCondition())
					gameState = LOSE_STATE;

			break;

			case WIN_STATE:
				showVictoryScreen(OFFSET_HEIGHT, BOARD_HEIGHT, OFFSET_WIDTH);
				if (userEnterAction) {
					clear();
					gameState = GAME_INTRODUCTION_STATE;
				}
			break;

			case LOSE_STATE:
				showFailureScreen(OFFSET_HEIGHT, BOARD_HEIGHT, OFFSET_WIDTH);
				if (userEnterAction) {
					clear();
					gameState = GAME_INTRODUCTION_STATE;
				}
			break;
		}
		delay(1);
	}

	ncursesEnd();
	return 0;
}

void ncursesInit() {
	initscr();
	curs_set(0);
	cbreak();
	noecho();
	keypad(stdscr,true);
	timeout(10);
}

void ncursesEnd() {
	endwin();
}

int getRandomIntegerInRange(int min,int max){
	return  min + getRandomInteger(max-min);
}

bool checkLoseCondition(){
	return !players[0].isAlive;
}

bool checkWinCondition(){
	for (int i = 1; i < PLAYERS_NUMBER; i++){
		if (players[i].isAlive) {
			return false;
		}
	}
	return !checkLoseCondition();
}
