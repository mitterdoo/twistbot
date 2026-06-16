#include <iostream>
#include <string>
#include <chrono>
#include <time.h>
#include <windows.h>
#include <WinUser.h>
#include "BejGame.h"
#include "TwistBot3.h"


// it's main
int main()
{

	BejGame* game = new BejGame();

	while (1)
	{
		Sleep(250);

		Vector2 cursor = game->GetCursor();
		if (GetKeyState(VK_F11) & 0x8000)
			break;

		
		SIZE_T boardAddr = game->ReadUInt(MEM_STATIC_BOARD);
		int movesAllowed = game->ReadUInt(boardAddr + MEM_OFFSET_CANMOVE);
		int levelScore = game->ReadUInt(boardAddr + MEM_OFFSET_LEVEL_SCORE);

		if (!movesAllowed && levelScore > 0)
			continue;

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

			Vector2 cursor = game->GetCursor();
			board->Rotate(cursor);
			board->RunMatch();

			printf("BOARD RESULT:\n");
			printf(board->GetString().c_str());
			printf("\n");
			delete board;
		}
		// */

		printf("=====================\n");
		high_resolution_clock::time_point t1 = high_resolution_clock::now();
		MoveCandidate curMove = FindBestMove(board, 10);
		high_resolution_clock::time_point t2 = high_resolution_clock::now();
		duration<double, std::milli> time_span = t2 - t1;
		printf("Calculated moves in %i MS.\n", (int)time_span.count());

		if (curMove.score > 0)
		{
			Vector2 pixelPos = board->gems[curMove.pos.x][curMove.pos.y].pixelPos;
			pixelPos.x += 128;
			pixelPos.y += 128;

			board->Rotate(curMove.pos);
			board->RunMatch();
			printf("SCORE: %i\n", curMove.score);

			game->Click(pixelPos);


		}

		delete board;
		

	}

	delete game;
	
}

MoveCandidate FindBestMove(Board* board, int level)
{
	MoveCandidate curMove = {{-1, -1}, 0};
	if (--level < 0) return curMove;

	for (int i = 0; i < level; i++)
	{
		//printf("---");
	}
	//printf("%i\n", level);
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
			nestedBoard->Rotate({x, y});
			int score = nestedBoard->RunMatch();
			//printf(nestedBoard->GetString().c_str());
			//printf("lvl %i\tscore: %i\n", level, score);
			if (score > 0)
			{
				MoveCandidate nestedMove = FindBestMove(nestedBoard, level);
				score += nestedMove.score;
			}

			if (score >= curMove.score)
			{
				curMove.pos = {x, y};
				curMove.score = score;
			}

			delete nestedBoard;

		}
	}
	return curMove;

}
