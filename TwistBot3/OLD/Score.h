#pragma once

typedef enum SCORES
{
	SPECIAL_INTERSECTION = 50,
	SPECIAL_KILL_FLAME = 100,
	SPECIAL_KILL_LIGHTNING = 300,
	SPECIAL_KILL_LIGHTNING_PERGEM = 20,
	SPECIAL_KILL_FRUIT = 1000,
	SPECIAL_KILL_FRUIT_PERGEM_NORMAL = 20,
	SPECIAL_KILL_FRUIT_PERGEM_FLAME = 200,
	SPECIAL_KILL_FRUIT_PERGEM_LIGHTNING = 300,
	SPECIAL_KILL_FRUIT_PERGEM_FRUIT = 2000,
	SPECIAL_KILL_BOMB = 100,
	SPECIAL_KILL_DOOM = 1000,
	SPECIAL_MEGAFRUIT = 71500
} SCORES;

class Score
{
public:
	Score();

	int score;
	int cascades;
	int coalCount;
	int multiplier = 1;

	void Reset();
	void ScoreMatch(int matchLength);
	void ScoreCascade();
	void ScoreCoal();
	void AddScore(int score);
	void AddScore(SCORES score);

};

