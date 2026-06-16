#include <algorithm>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include "Board.h"
#include "MonteCarlo.h"
using namespace std::chrono;
using namespace std;
typedef struct MoveCandidate
{
	Vector2 pos;
	int score;
	Board* board;
} MoveCandidate;

typedef struct MoveResult
{
	Vector2 pos;
	int score;
	int totalScore;
	int levels;
};

bool sortMove(MoveCandidate a, MoveCandidate b)
{
	return a.score > b.score;
}

MoveResult FindBestMove(Board* board, int level, int scoreToBeat=LOWEST_SCORE);
