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
	int level_score = 0;
	int cascades = 0;
	int coal_count = 0;
	int multiplier = 1;

	void reset();
	void score_match(int matchLength);
	void score_cascade();
	void score_coal();
	void add_score(int score, const char* msg = "");
	void add_score(SCORES score, const char* msg = "");
	void add_score_no_multiplier(int score, const char* msg = "");
	void add_score_no_multiplier(SCORES score, const char* msg = "");

private:
	int level_score_accumulator = 0;
	void add_level_score(int baseScore, int score);

};

