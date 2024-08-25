#include <iostream>
#include <string>
#include <chrono>
#include <time.h>
#include <windows.h>
#include <WinUser.h>
#include "BejGame.h"
#include "TwistBot4.h"

////////////////////// TODO: WORK FOR CLASSIC MODE AND SPECIAL GEMS
/////////////////////TODO:
/*
* 
*	
*	Don't allow a non-move to be made unless it will result in defusing a bomb
* 
* 
*	Meme bot
*	
*	Twist the board like a rubik's cube until bonus move is possible	
* 
*/

// it's main
int main()
{

	//srand(time(NULL));
	srand(54);

	BejGame* game = new BejGame();

	int _printedNoMoves = 0;

	while (1)
	{
		Sleep(50);

		Vector2 cursor = game->GetCursor();
		if (GetKeyState(VK_F11) & 0x8000)
			break;

		
		SIZE_T boardAddr = game->ReadUInt(MEM_STATIC_BOARD);
		int movesAllowed = game->ReadUInt(boardAddr + MEM_OFFSET_CANMOVE);
		int levelScore = game->ReadUInt(boardAddr + MEM_OFFSET_LEVEL_SCORE);

		if (!movesAllowed && levelScore > 0)
		{
			if (!_printedNoMoves)
			{
				_printedNoMoves = 1;
				printf("No moves allowed. waiting...\n");
			}
			continue;
		}
		_printedNoMoves = 0;

		game->WaitForStationaryGems();
		//game->WaitForBonusComplete();

		Board* board = game->FetchBoard();
		if (board == NULL)
		{
			continue;
		}


		/*
		{
			if (GetKeyState(VK_F8) & 0x8000)
			{
				Vector2 cursor = game->GetCursor();
				game->SetGemFlag(cursor, GemFlags::FLAME);
			}
			if (GetKeyState(VK_F9) & 0x8000)
			{
				Vector2 cursor = game->GetCursor();
				game->SetGemColor(cursor, GemColor::RED);
			}

			printf("Combo BEFORE: %i @ %ix\n", board->comboMeter.count, board->comboMeter.multiplier);
			Vector2 cursor = game->GetCursor();
			board->Rotate(cursor);
			int boardScore = board->RunMatch();

			printf("BOARD RESULT:\n");
			printf(board->GetString().c_str());
			printf("Combo: %i @ %ix\n", board->comboMeter.count, board->comboMeter.multiplier);
			printf("Score: %i\n", boardScore);
			printf("\n");
		}
		// */


		if (GetKeyState(VK_F10) & 0x8000)
			printf("pause\n");
		
		printf("=====================\n");
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		//MoveResult curMove = FindBestMove(board, 3);

		Vector2 curMove = MonteCarlo::RunAlgorithm(board, 3000);


		high_resolution_clock::time_point t2 = high_resolution_clock::now();
		duration<double, std::milli> time_span = t2 - t1;
		printf("Calculated moves in %i MS.\n", (int)time_span.count());

		Vector2 pixelPos = board->gems[curMove.x][curMove.y].pixelPos;

		pixelPos.x += 128;
		pixelPos.y += 128;

		board->Rotate(curMove);
		board->RunMatch();
		printf("\n");
		printf("===FINAL MOVE: (%i, %i)===\n", curMove.x, curMove.y);

		game->Click(pixelPos);
		Sleep(300);
		



		delete board;
		

	}

	delete game;
	
}

MoveResult FindBestMove(Board* board, int level, int scoreToBeat)
{
	level--;
	vector<MoveCandidate> moveCandidates;
	// find all legal moves

	for (int x = 0; x < 7; x++)
	{
		for (int y = 0; y < 7; y++)
		{
			if (board->gems[x][y].color == GemColor::EMPTY &&
				board->gems[x + 1][y].color == GemColor::EMPTY &&
				board->gems[x + 1][y + 1].color == GemColor::EMPTY &&
				board->gems[x][y + 1].color == GemColor::EMPTY)
				continue;

			Board* nestedBoard = board->Copy();
			if (!nestedBoard->Rotate({x, y}))
			{
				delete nestedBoard;
				continue;
			}
			int score = nestedBoard->RunMatch();
			//printf(nestedBoard->GetString().c_str());
			//printf("lvl %i\tscore: %i\n", level, score);
			if (score > LOWEST_SCORE)
			{
				//printf("scores above ^\n");
				for (int i = 0; i <= level; i++)
				{
					//printf("==");
				}
				//printf("%i\n", level);
				//printf("move score: %i\n", score);
				//printf(nestedBoard->GetString().c_str());
				//printf("\n\n");

				moveCandidates.push_back({{x, y}, score, nestedBoard});
			}
			else
			{
				delete nestedBoard;
			}

		}
	}

	if (moveCandidates.size() == 0)
	{
		return {{-1, -1}, LOWEST_SCORE, 0, 0};
	}

	MoveCandidate front = moveCandidates.front();
	if (level == 0)
	{
		for (MoveCandidate move : moveCandidates)
			delete move.board;

		return {front.pos, front.score, front.score, 0};
	}

	MoveResult bestMove = {{-1, -1}, LOWEST_SCORE, 0, 0};

	for (MoveCandidate move : moveCandidates)
	{

		MoveResult nested = FindBestMove(move.board, level, bestMove.score);
		if (nested.score >= bestMove.score)
		{
			bestMove.pos = move.pos;
			bestMove.score = (nested.score == LOWEST_SCORE ? 0 : nested.score) + move.score;
			bestMove.levels = nested.levels + 1;
			//bestMove.totalScore = nested.totalScore + move.score;
		}

		delete move.board;
	}

	return bestMove;



	/*
	level--;
	vector<MoveCandidate> moveCandidates;
	// find all legal moves

	for (int x = 0; x < 7; x++)
	{
		for (int y = 0; y < 7; y++)
		{
			if (board->gems[x][y].color == GemColor::EMPTY &&
				board->gems[x + 1][y].color == GemColor::EMPTY &&
				board->gems[x + 1][y + 1].color == GemColor::EMPTY &&
				board->gems[x][y + 1].color == GemColor::EMPTY)
				continue;

			Board* nestedBoard = board->Copy();
			if (!nestedBoard->Rotate({x, y}))
			{
				delete nestedBoard;
				continue;
			}
			int score = nestedBoard->RunMatch();
			//printf(nestedBoard->GetString().c_str());
			//printf("lvl %i\tscore: %i\n", level, score);
			if (score > LOWEST_SCORE)
			{
				//printf("scores above ^\n");
				for (int i = 0; i <= level; i++)
				{
					//printf("==");
				}
				//printf("%i\n", level);
				//printf("move score: %i\n", score);
				//printf(nestedBoard->GetString().c_str());
				//printf("\n\n");
				
				moveCandidates.push_back({{x, y}, score, nestedBoard});
			}
			else
			{
				delete nestedBoard;
			}
			
		}
	}

	if (moveCandidates.size() == 0)
	{
		return {{-1, -1}, LOWEST_SCORE, 0, 0};
	}

	sort(moveCandidates.begin(), moveCandidates.end(), sortMove);

	MoveCandidate front = moveCandidates.front();
	if (level == 0 || front.score < scoreToBeat)
	{
		for (MoveCandidate move : moveCandidates)
			delete move.board;

		return {front.pos, front.score, front.score, 0};
	}

	MoveResult bestMove = {{-1, -1}, LOWEST_SCORE, 0, 0};

	for (MoveCandidate move : moveCandidates)
	{

		MoveResult nested = FindBestMove(move.board, level, bestMove.score);
		if (nested.score >= bestMove.score)
		{
			bestMove.pos = move.pos;
			bestMove.score = (nested.score == LOWEST_SCORE ? 0 : nested.score) + move.score;
			bestMove.levels = nested.levels + 1;
			//bestMove.totalScore = nested.totalScore + move.score;
		}

		delete move.board;
	}

	return bestMove;
	*/


}
