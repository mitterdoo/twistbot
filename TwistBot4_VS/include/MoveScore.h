#pragma once
#include <math.h>

typedef enum SCORES
{

	SPECIAL_KILL_FLAME = 50,
	SPECIAL_KILL_LIGHTNING = 150,
	SPECIAL_KILL_LIGHTNING_PERGEM = 10,
	SPECIAL_KILL_FRUIT = 500,
	SPECIAL_KILL_FRUIT_PERGEM_NORMAL = 10,
	SPECIAL_KILL_FRUIT_PERGEM_FLAME = 100,
	SPECIAL_KILL_FRUIT_PERGEM_LIGHTNING = 150,
	SPECIAL_KILL_FRUIT_PERGEM_FRUIT = 1000,
	SPECIAL_KILL_BOMB = 50,
	SPECIAL_KILL_DOOM = 500,
	SPECIAL_MEGAFRUIT = 35750,
	SPECIAL_CREATE_FLAME = 500,
	SPECIAL_CREATE_LIGHTNING = 1000,
	SPECIAL_CREATE_NOVA = 2500,
	SPECIAL_CREATE_FRUIT = 3750,

	/*

	The game does not reward any points for:
		* Destroying locking/locked gems

	*/

} SCORES;


// Accumulates the scores for one move
class MoveScore
{
public:
	MoveScore();

	int score = 0;
	int levelScore = 0;
	int cascades = 0;
	int coalCount = 0;
	int multiplier = 1;

	void Reset();
	void ScoreMatch(int matchLength);
	void ScoreCascade();
	void ScoreCoal();
	void AddScore(int score, const char* msg = "");
	void AddScore(SCORES score, const char* msg = "");
	void AddScoreNoMultiplier(int score, const char* msg = "");
	void AddScoreNoMultiplier(SCORES score, const char* msg = "");

private:
	int levelScoreAccumulator = 0;
	void AddLevelScore(int baseScore, int score);

};

